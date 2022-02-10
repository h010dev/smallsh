/**
 * @file job-control.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Functions for performing shell job control.
 *
 * Some of the ideas presented here were retrieved from the following sources:
 * https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 * https://www.gnu.org/software/libc/manual/html_node/Freground-and-Background.html
 * https://www.gnu.org/software/libc/manual/html_node/Stopped-and-Terminated-Jobs.html
 */
#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "globals.h"

#ifdef DEBUG
#include <string.h>
#endif

#include "job-control/job-control.h"

void job_control_wait_for_job(Job *job)
{
        int exit_status;
        pid_t child;
        siginfo_t info;
        int opt;
        bool sigtstp_raised;
        int status;

        opt = WEXITED | WSTOPPED | WNOWAIT; /* don't collect child */
        sigtstp_raised = false;

        /*
         * Loop until the child process is terminated due to any signal but
         * SIGTSTP. If SIGTSTP is raised, take note and re-raise it once
         * child process is terminated, so that the shell is later notified
         * via its SIGTSTP handler.
         */
        for (;;) {
                errno = 0;
                child = waitid(P_PID, job->job_proc->proc_pid, &info, opt);
                if (child == -1 && errno != ECHILD) {
                        perror("waitid");
                        _exit(1);
                }

                /* Child was stopped. */
                if (info.si_code == CLD_STOPPED) {
                        if (info.si_status == SIGTSTP) {
                                sigtstp_raised = true;

                                /*
                                 * We want to ignore SIGTSTP, so collect child,
                                 * resume it, and wait again.
                                 */
                                errno = 0;
                                child = waitpid(child, &exit_status, 0);
                                if (child == -1 && errno != ECHILD) {
                                        perror("waitpid");
                                        _exit(1);
                                }

                                errno = 0;
                                status = kill(child, SIGCONT);
                                if (status == -1) {
                                        perror("kill");
                                        _exit(1);
                                }
                        } else {
                                exit_status = info.si_status;
                                break;
                        }
                }
                /* Child was terminated (normally or via a signal). */
                else {
                        exit_status = info.si_status;
                        break;
                }
        }

        /*
         * Re-raise SIGTSTP signal to shell. This is done because signal
         * handlers are removed when child execs a program, so all we get is a
         * SIGTSTP signal with default behavior.
         */
        if (sigtstp_raised) {
                errno = 0;
                status = kill(getpid(), SIGTSTP);
                if (status == -1) {
                        perror("kill");
                        _exit(1);
                }
        }

        /*
         * Mark job as completed and update its status so that it can be later
         * removed from the job table and relayed to user.
         */
        job->job_proc->proc_completed = true;
        job->job_proc->proc_status = exit_status;
        smallsh_status = exit_status;
}

void job_control_foreground_job(Job *job)
{
        int status;

#ifdef DEBUG
        printf("0: tcgrp=%d\n", tcgetpgrp(shell_terminal));
#endif

        /* Put the job into the foreground. */
        errno = 0;
        status = tcsetpgrp(shell_terminal, job->job_pgid);
        if (status == -1) {
                perror("tcsetpgrp");
                _exit(1);
        }

#ifdef DEBUG
        printf("1: tcgrp=%d\n", tcgetpgrp(shell_terminal));
#endif

        /* Wait for it to report. */
        job_control_wait_for_job(job);

        /* Put the shell back in the foreground. */
        errno = 0;
        status = tcsetpgrp(shell_terminal, shell_pgid);
        if (status == -1) {
                perror("tcsetpgrp");
                _exit(1);
        }

#ifdef DEBUG
        printf("2: tcgrp=%d\n", tcgetpgrp(shell_terminal));
#endif
}

int job_control_launch_job(Job **job, bool foreground)
{
        int status;
        Job *job_;
        pid_t spawn_pid;

        job_ = *job;

        spawn_pid = fork();
        if (spawn_pid == 0) {
                process_launch(job_->job_proc, job_->job_pgid, job_->job_stdin,
                               job_->job_stdout, foreground);

                /* If we reach this point, an error occurred. */
                _exit(1);
                return -1;
        } else if (spawn_pid < 0) {
                perror("fork");
                _exit(1);
        } else {
                /* Put job into its own group and make it the process leader. */
                job_->job_proc->proc_pid = spawn_pid;
                if (job_->job_pgid == 0) {
                        job_->job_pgid = spawn_pid;
                }

                errno = 0;
                status = setpgid(spawn_pid, job_->job_pgid);
                if (status == -1 && errno != EACCES) {
                        perror("setpgid");
                        _exit(1);
                }
        }

        if (foreground) {
                if (shell_is_interactive) {
                        /*
                         * Give job control over foreground if we are in
                         * interactive mode.
                         */
                        job_control_foreground_job(job_);
                } else {
                        /* Otherwise, just wait for job to complete. */
                        job_control_wait_for_job(job_);
                }
        }
        /* Background job; do nothing. */

        return 0;
}
