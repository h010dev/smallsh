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
#include <termios.h>
#include <unistd.h>

#include "globals.h"
#include "error.h"

#ifdef DEBUG
#include <string.h>
#endif

#include "job-control/job-control.h"

/* *****************************************************************************
 * PUBLIC DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
static void SH_JobControlWaitForJob(SH_Job *job);

static void SH_JobControlBGJob(SH_Job *job)
{
        fprintf(stdout, "[%d]\t%d\n", job->spec, job->proc->pid);
        fflush(stdout);
}

/**
 * @brief Run @p job in the foreground.
 *
 * Puts @p job into the foreground, waits for it to complete, then puts
 * shell back into foreground.
 * @param job job to run
 */
static void SH_JobControlFGJob(SH_Job *job)
{
        int status;

        /* Put the job into the foreground. */
        errno = 0;
        status = tcsetpgrp(smallsh_shell_terminal, job->pgid);
        if (status == -1) {
                perror("tcsetpgrp");
                _exit(1);
        }

        /* Wait for it to report. */
        SH_JobControlWaitForJob(job);

        /* Put the shell back in the foreground. */
        errno = 0;
        status = tcsetpgrp(smallsh_shell_terminal, smallsh_shell_pgid);
        if (status == -1) {
                perror("tcsetpgrp");
                _exit(1);
        }

        /* Clear terminal buffers. */
        tcflush(smallsh_shell_terminal, TCIOFLUSH);
}

static void SH_JobControlWaitForJob(SH_Job *job)
{
        int exit_status;
        pid_t child;
        siginfo_t info;
        int opt;
        bool sigtstp_raised, normal_termination;
        int status;

        opt = WEXITED | WSTOPPED | WNOWAIT; /* don't collect child */
        sigtstp_raised = false;
        normal_termination = false;
        exit_status = 0;

        /*
         * Loop until the child process is terminated due to any signal but
         * SIGTSTP. If SIGTSTP is raised, take note and re-raise it once
         * child process is terminated, so that the shell is later notified
         * via its SIGTSTP handler.
         */
        for (;;) {
                errno = 0;
                child = waitid(P_PID, job->proc->pid, &info, opt);
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
                /* Child was terminated normally. */
                else if (info.si_code == CLD_EXITED) {
                        exit_status = info.si_status;
                        normal_termination = true;
                        break;
                }
                /* Child was terminated by signal. */
                else {
                        exit_status = info.si_status;
                        break;
                }
        }

        if (WIFSIGNALED(exit_status) && !normal_termination) {
                fprintf(stdout, "terminated by signal %d\n", WTERMSIG(exit_status));
                fflush(stdout);
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
        job->proc->has_completed = true;
        job->proc->status = exit_status;
        smallsh_errno = exit_status;
}

/* *****************************************************************************
 * PUBLIC DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
int SH_JobControlLaunchJob(SH_Job **job, bool run_fg)
{
        int status;
        SH_Job *job_;
        pid_t spawn_pid;

        job_ = *job;

        spawn_pid = fork();
        if (spawn_pid == 0) {
                SH_LaunchProcess(job_->proc, job_->pgid,
                                 job_->infile,
                                 job_->outfile, run_fg);

                /* If we reach this point, an error occurred. */
                _exit(1);
        } else if (spawn_pid < 0) {
                perror("fork");
                _exit(1);
        } else {
                /* Put job into its own group and make it the process leader. */
                job_->proc->pid = spawn_pid;
                if (job_->pgid == 0) {
                        job_->pgid = spawn_pid;
                }

                errno = 0;
                status = setpgid(spawn_pid, job_->pgid);
                if (status == -1 && errno != EACCES) {
                        perror("setpgid");
                        _exit(1);
                }
        }

        /* Foreground job. */
        if (run_fg) {
                if (smallsh_interactive_mode) {
                        /*
                         * Give job control over foreground if we are in
                         * interactive mode.
                         */
                        SH_JobControlFGJob(job_);
                } else {
                        /* Otherwise, just wait for job to complete. */
                        SH_JobControlWaitForJob(job_);
                }
        }
        /* Background job. */
        else {
                SH_JobControlBGJob(job_);
        }

        return 0;
}
