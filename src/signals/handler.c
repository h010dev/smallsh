/**
 * @file handler.c
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief Contains functions for handling signals.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "signals/handler.h"
#include "core/dtos.h"
#include "core/shell-attrs.h"

void handler_handle_sigtstp(int sig)
{
        (void) sig;

        int saved_errno;

        /* save errno */
        saved_errno = errno;

#ifdef DEBUG
        write(STDOUT_FILENO, "sigtstp received\n", 17);
#endif

        /*
         * Write a single byte to pipe to notify listener that SIGTSTP was
         * caught.
         */
        errno = 0;
        if (write(pipe_sigtstp[1], "x", 1) == -1 && errno != EAGAIN) {
                perror("write");
                _exit(1);
        }

        /* reset errno */
        errno = saved_errno;
}

void handler_handle_sigchld(int sig)
{
        (void) sig;

        int status, saved_errno;
        pid_t child_pid;
        SigchldDTO dto;

        /* save errno */
        saved_errno = errno;

        /*
         * Catch all incoming SIGCHLD signals and dispatch their status to
         * listening parties.
         *
         * Source: TLPI section 26.3
         */
        errno = 0;
        while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
                /*
                 * Initialize DTO object to transfer PID and status through.
                 */
                dto.chld_pid = child_pid;
                dto.chld_status = status;

#ifdef DEBUG
                write(STDERR_FILENO, "sigchld received\n", 18);
#endif

                /*
                 * Write DTO to pipe.
                 */
                errno = 0;
                if (write(pipe_sigchld[1], &dto, sizeof(dto)) == -1 && errno != EAGAIN) {
                        perror("write");
                        _exit(1);
                }
        }
        if (child_pid == -1 && errno != ECHILD) {
                perror("waitpid");
                _exit(1);
        }

        /* reset errno */
        errno = saved_errno;
}
