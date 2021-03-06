/**
 * @file handler.c
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief Contains functions for handling signals.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "signals/handler.h"
#include "events/events.h"

/* *****************************************************************************
 * PRIVATE DEFINITIONS
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
/**
 * @brief Switches SIGTSTP mask to disable fg_only_mode on next receipt.
 *
 * Source: https://edstem.org/us/courses/16718/discussion/1067170
 */
static void handler_switch_disable_fg_only_mode(void)
{
        struct sigaction sa;
        sigset_t block_set;
        int status;

        /* Ignore SIGCHLD signals within this handler. */
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
        sa.sa_handler = SH_HandlerDisableFgOnlyMode;

        errno = 0;
        status = sigaction(SIGTSTP, &sa, NULL);
        if (status == -1) {
                perror("sigaction");
                _exit(1);
        }
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
 * OBJECTS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
sig_atomic_t smallsh_fg_only_mode_flag = 0;

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
void SH_HandlerDisableFgOnlyMode(int sig)
{
        (void) sig;

        char fg_off[] = "\nExiting foreground-only mode\n";

        smallsh_fg_only_mode_flag = 0;
        write(STDOUT_FILENO, fg_off, sizeof(fg_off) - 1);
        SH_HandlerSwitchEnableFgOnlyMode();
}

void SH_HandlerEnableFgOnlyMode(int sig)
{
        (void) sig;

        char fg_on[] = "\nEntering foreground-only mode (& is now ignored)\n";

        smallsh_fg_only_mode_flag = 1;
        write(STDOUT_FILENO, fg_on, sizeof(fg_on) - 1);
        handler_switch_disable_fg_only_mode();
}

void SH_HandlerHandleSigchld(int sig)
{
        (void) sig;

        int status, saved_errno;

        saved_errno = errno;

        status = SH_SenderNotifySigchldEvent(sigchld_channel);
        if (status == -1) {
                fprintf(stderr, "SH_SenderNotifySigchldEvent()");
                fflush(stderr);
                _exit(1);
        }

        errno = saved_errno;
}

void SH_HandlerSwitchEnableFgOnlyMode(void)
{
        struct sigaction sa;
        sigset_t block_set;
        int status;

        /* Ignore SIGCHLD signals within this handler. */
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
        sa.sa_handler = SH_HandlerEnableFgOnlyMode;

        errno = 0;
        status = sigaction(SIGTSTP, &sa, NULL);
        if (status == -1) {
                perror("sigaction");
                _exit(1);
        }
}
