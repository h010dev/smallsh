/**
 * @file sender.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Responsible for sending messages to subscribers via a dedicated Channel.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "events/sender.h"
#include "events/dto.h"

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
 * CONSTRUCTORS / DESTRUCTORS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
int sender_ctor(Sender *self, size_t max_ch)
{
        self->_snd_n_ch = 0;
        self->_snd_max_ch = max_ch;

        /* Init Channel array. */
        self->_ch = malloc(max_ch * sizeof(Channel));
        if (self->_ch == NULL) {
                fprintf(stderr, "Failed to init Sender: malloc()\n");
                return -1;
        }

        for (size_t i = 0; i < max_ch; i++) {
                self->_ch[i] = NULL;
        }

        return 0;
}

void sender_dtor(Sender *self)
{
        for (size_t i = 0; i < self->_snd_n_ch; i++) {
                self->_ch[i] = NULL;
        }
        self->_snd_n_ch = 0;

        free(self->_ch);
        self->_ch = NULL;
}

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
int sender_add(Sender *self, Channel *ch)
{
        int flags, status;

        /* Set write end to non-blocking. */
        errno = 0;
        flags = fcntl(ch->ch_write, F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "Failed to add to Sender: %s\n", strerror(errno));
                return -1;
        }
        flags |= O_NONBLOCK;

        errno = 0;
        status = fcntl(ch->ch_write, F_SETFL, flags);
        if (status == -1) {
                fprintf(stderr, "Failed to add to Sender: %s\n", strerror(errno));
                return -1;
        }

        /* Add channel to channel list. */
        if (self->_snd_n_ch >= self->_snd_max_ch) {
                fprintf(stderr, "Failed to add to Sender: Sender is full\n");
                return -1;
        }

        self->_ch[self->_snd_n_ch++] = ch;

        return 0;
}

int sender_notify_sigchld(struct Channel ch)
{
        int status;
        pid_t child_pid;
        SigchldDTO dto;

        /*
         * Catch all incoming SIGCHLD signals and dispatch their status to
         * listening parties.
         *
         * Source: TLPI section 26.3
         */
        errno = 0;
        while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
                /* Initialize DTO object to transfer PID and status through. */
                dto.chld_pid = child_pid;
                dto.chld_status = status;

                /* Write DTO to pipe. */
                errno = 0;
                if (write(ch.ch_write, &dto, sizeof(dto)) == -1 && errno != EAGAIN) {
                        return -1;
                }
        }
        if (child_pid == -1 && errno != ECHILD) {
                return -1;
        }

        return 0;
}
