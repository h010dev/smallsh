/**
 * @date 05 Feb 2022
 */
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

#include "job-control/job-control.h"
#include "job-control/job-table.h"
#include "core/shell-attrs.h"

JobTable job_table;

void job_control_foreground_job(Job *job, int cont)
{
        /* put the job into the foreground */
        tcsetpgrp(shell_terminal, job->job_pgid);

        /* send the job a continue signal, if necessary */
        if (cont) {
                tcsetattr(shell_terminal, TCSADRAIN, &job->tmodes);
                if (kill(-job->job_pgid, SIGCONT) < 0) {
                        perror("kill(SIGCONT)");
                }
        }

        /* wait for it to report */

        /* put the shell back in the foreground */
        tcsetpgrp(shell_terminal, shell_pgid);

        /* restore the shell's terminal modes */
        tcgetattr(shell_terminal, &job->tmodes);
        tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
}

void job_control_background_job(Job *job, int cont)
{
        /* send the job a continue signal, if necessary */
        if (cont) {
                if (kill(-job->job_pgid, SIGCONT) < 0) {
                        perror("kill(SIGCONT)");
                }
        }
}

void job_control_launch_job(Job **job, bool foreground)
{
        Job *job_ = *job;

        pid_t spawn_pid = fork();
        if (spawn_pid == 0) {
                process_launch(&job_->job_proc, job_->job_pgid, job_->job_stdin,
                               job_->job_stdout, foreground);
        } else if (spawn_pid < 0) {
                perror("fork");
                _exit(1);
        } else {
                job_->job_proc.proc_pid = spawn_pid;
                if (shell_is_interactive) {
                        if (job_->job_pgid == 0) {
                                job_->job_pgid = spawn_pid;
                        }
                        setpgid(spawn_pid, job_->job_pgid);
                }
        }

        if (!shell_is_interactive) {
                /* wait for job */
                int exit_status;
                pid_t child = waitpid(spawn_pid, &exit_status, 0);
                (void) child;
        } else if (foreground) {
                job_control_foreground_job(job_, false);
        } else {
                job_control_background_job(job_, false);
        }
}
