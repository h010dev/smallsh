/**
 * @file handler.c
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief Contains functions for handling signals.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "signals/handler.h"
#include "events/events.h"

void handler_handle_sigchld(int sig)
{
        (void) sig;

        int status, saved_errno;

        saved_errno = errno;

        status = sender_notify_sigchld(ch_sigchld);
        if (status == -1) {
                fprintf(stderr, "sender_notify_sigchld()");
                fflush(stderr);
                _exit(1);
        }

        errno = saved_errno;
}

void handler_handle_sigtstp(int sig)
{
        (void) sig;

        int status;
        int saved_errno;

        saved_errno = errno;

        status = sender_notify_sigtstp(ch_sigtstp);
        if (status == -1) {
                fprintf(stderr, "sender_notify_sigtstp()");
                fflush(stderr);
                _exit(1);
        }

        errno = saved_errno;
}
