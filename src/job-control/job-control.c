/**
 * @file job-control.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Functions for performing shell job control.
 *
 * Ideas presented here were retrieved from the following sources:
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

#ifdef DEBUG
#include <string.h>
#endif

#include "job-control/job-control.h"
#include "core/shell-attrs.h"
#include "core/error.h"

void job_control_wait_for_job(Job *job)
{
        int exit_status;
        pid_t child;
        siginfo_t info;
        int opt;
        bool sigtstp_raised;
        int status;

#ifdef DEBUG
        setbuf(stderr, NULL);
#endif

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
                if (child == -1) {
                        /* Error ! */
                        perror("waitid");
                        _exit(1);
                }

                if (info.si_code == CLD_STOPPED) {
                        if (info.si_status == SIGTSTP) {
                                sigtstp_raised = true;
#ifdef DEBUG
                                fprintf(stderr, "CLD_STOPPED & SIGTSTP\n");
#endif
                                /*
                                 * We want to ignore SIGTSTP, so collect child
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
                                        /* Error ! */
                                        perror("kill");
                                        _exit(1);
                                }
                        } else {
#ifdef DEBUG
                                fprintf(stderr, "CLD_STOPPED\n");
#endif
                                exit_status = info.si_status;
                                break;
                        }
                } else {
#ifdef DEBUG
                        fprintf(stderr, "OTHER\n");
#endif
                        exit_status = info.si_status;
                        break;
                }
        }

        /*
         * Re-raise sigtstp signal to stopped child. This is done because
         * signal handlers are removed when child execs a program, so all
         * we get is a sigtstp with default behavior.
         */
        if (sigtstp_raised) {
                errno = 0;
                status = kill(getpid(), SIGTSTP);
                if (status == -1) {
                        /* Error ! */
                        perror("kill");
                        _exit(1);
                }
        }

        /*
         * Mark job as completed and update its status
         * so that it can be later removed from the job table and relayed
         * to user.
         */
        job->job_proc->proc_completed = true;
        job->job_proc->proc_status = exit_status;
        smallsh_status = exit_status;

#ifdef DEBUG
        if (WIFSIGNALED(exit_status)) {
                int sig = WTERMSIG(exit_status);
                if (sig == SIGINT) {
                        fprintf(stderr, "sigint received after sgtstp\n");
                } else if (sig == SIGTSTP) {
                        fprintf(stderr, "sigtstp received now\n");
                }
        } else if (WIFEXITED(exit_status)) {
                fprintf(stderr, "exited normally\n");
        } else {
                // error
        }
#endif
}

void job_control_foreground_job(Job *job)
{
        int status;
#ifdef DEBUG
        printf("0: tcgrp=%d\n", tcgetpgrp(shell_terminal));
#endif

        /* put the job into the foreground */
        errno = 0;
        status = tcsetpgrp(shell_terminal, job->job_pgid);
        if (status == -1) {
                perror("tcsetpgrp");
                _exit(1);
        }

#ifdef DEBUG
        printf("1: tcgrp=%d\n", tcgetpgrp(shell_terminal));
#endif

        /* wait for it to report */
        job_control_wait_for_job(job);

        /* put the shell back in the foreground */
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
#ifdef DEBUG
                sigset_t block_set;

                sigemptyset(&block_set);
                sigaddset(&block_set, SIGTSTP);

                sigprocmask(SIG_BLOCK, &block_set, NULL);
#endif

                process_launch(job_->job_proc, job_->job_pgid, job_->job_stdin,
                               job_->job_stdout, foreground);

                /*
                 * If we reach this point, an error occurred.
                 */
                return -1;

#ifdef DEBUG
                sigset_t pending;
                sigpending(&pending);
                if (sigismember(&pending, SIGTSTP)) {
                        printf("sigtstp pending...\n");
                }
                if (sigismember(&pending, SIGINT)) {
                        printf("sigint pending...\n");
                }
                if (sigismember(&pending, SIGCHLD)) {
                        printf("sigchld pending...\n");
                }

                sigprocmask(SIG_UNBLOCK, &block_set, NULL);
#endif
        } else if (spawn_pid < 0) {
                perror("fork");
                _exit(1);
        } else {
#ifdef DEBUG
                printf("parent: pid=%d\n", spawn_pid);
#endif
                job_->job_proc->proc_pid = spawn_pid;
                if (shell_is_interactive) {
                        if (job_->job_pgid == 0) {
                                job_->job_pgid = spawn_pid;
                        }

                        errno = 0;
                        status = setpgid(spawn_pid, job_->job_pgid);
                        if (status == -1) {
                                perror("setpgid");
                                _exit(1);
                        }
#ifdef DEBUG
                        printf("parent: pgid=%d\n", getpgid(spawn_pid));
#endif
                }
        }

        if (!shell_is_interactive) {
                job_control_wait_for_job(job_);
        } else if (foreground) {
                job_control_foreground_job(job_);
        } else {
                /* background job; do nothing */
        }

        return 0;
}
