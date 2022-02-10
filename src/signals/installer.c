/**
 * @file installer.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contains functions for installing signals.
 *
 * Ideas presented here were retrieved from the following sources:
 * https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 * https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 */
#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "signals/installer.h"
#include "signals/handler.h"

void installer_install_job_control_signals(void)
{
        sighandler_t status;

        /* Ignore interactive and job-control signals. */
        errno = 0;
        status = signal(SIGINT, SIG_IGN);
        if (status == SIG_ERR) {
                perror("signal");
                _exit(1);
        }

        /*
         * Shell should ignore read/write signals since it is allowed to
         * perform IO.
         */
        errno = 0;
        status = signal(SIGTTIN, SIG_IGN);
        if (status == SIG_ERR) {
                perror("signal");
                _exit(1);
        }

        errno = 0;
        status = signal(SIGTTOU, SIG_IGN);
        if (status == SIG_ERR) {
                perror("signal");
                _exit(1);
        }

        /* Shell should handle SIGCHLD events for later delivery. */
        installer_install_sigchld_handler();

        /* Shell should handle SIGTSTP events to enable/disable fg only mode. */
        installer_install_sigtstp_handler();
}

void installer_install_child_process_signals(bool foreground)
{
        sighandler_t sig_status;

        /* Allow SIGINT to terminate this process. */
        errno = 0;
        sig_status = signal(SIGINT, SIG_DFL);
        if (sig_status == SIG_ERR) {
                perror("signal");
                _exit(1);
        }

        /*
         * Background process groups should exhibit default behavior (i.e. stop)
         * when attempting to read/write to terminal.
         *
         * Since background process groups are by default redirected to
         * /dev/null by this shell if they do not explicitly specify input/output
         * redirections, SIGTTIN/SIGTTOU signals should not be emitted in
         * general. However, it helps to be explicit that this is the expected
         * behavior with the code segment below.
         */
        if (!foreground) {
                errno = 0;
                sig_status = signal(SIGTTIN, SIG_DFL);
                if (sig_status == SIG_ERR) {
                        perror("signal");
                        _exit(1);
                }

                errno = 0;
                sig_status = signal(SIGTTOU, SIG_DFL);
                if (sig_status == SIG_ERR) {
                        perror("signal");
                        _exit(1);
                }
        }
}

void installer_install_sigchld_handler(void)
{
        struct sigaction sa;
        sigset_t block_set;
        int status;

        /* Ignore SIGTSTP signals within this handler. */
        memset(&sa, 0, sizeof(sa));

        errno = 0;
        status = sigemptyset(&block_set);
        if (status == -1) {
                perror("sigemptyset");
                _exit(1);
        }

        errno = 0;
        status = sigaddset(&block_set, SIGTSTP);
        if (status == -1) {
                perror("sigaddset");
                _exit(1);
        }

        sa.sa_mask = block_set;
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = handler_handle_sigchld;

        errno = 0;
        status = sigaction(SIGCHLD, &sa, NULL);
        if (status == -1) {
                perror("sigaction");
                _exit(1);
        }
}

void installer_install_sigtstp_handler(void)
{
        handler_install_fg_only_mode();

        /*
        struct sigaction sa;
        sigset_t block_set;
        int status;

        // Ignore SIGCHLD signals within this handler.
        memset(&sa, 0, sizeof(sa));

        errno = 0;
        status = sigemptyset(&block_set);
        if (status == -1) {
                perror("sigemptyset");
                _exit(1);
        }

        errno = 0;
        status = sigaddset(&block_set, SIGCHLD);
        if (status == -1) {
                perror("sigaddset");
                _exit(1);
        }

        sa.sa_mask = block_set;
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = handler_handle_sigtstp;

        errno = 0;
        status = sigaction(SIGTSTP, &sa, NULL);
        if (status == -1) {
                perror("sigaction");
                _exit(1);
        }
        */
}
