/**
 * @file receiver.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Responsible for receiving and handling signal-generated messages sent
 * via a dedicated Channel.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "events/receiver.h"
#include "events/dto.h"
#include "job-control/job-control.h"

#include "globals.h"

int receiver_ctor(Receiver *self, size_t max_ch)
{
        fd_set read_fds;

        self->_rcv_n_fds = 0;
        self->_rcv_n_ch = 0;
        self->_rcv_max_ch = max_ch;

        /* Init Channel array. */
        self->_ch = malloc(max_ch * sizeof(Channel));
        if (self->_ch == NULL) {
                fprintf(stderr, "Failed to init Receiver: malloc()\n");
                return -1;
        }

        for (size_t i = 0; i < max_ch; i++) {
                self->_ch[i] = NULL;
        }

        /* Zero-out read fds set */
        FD_ZERO(&read_fds);
        self->_rcv_fds = read_fds;

        return 0;
}

void receiver_dtor(Receiver *self)
{
        self->_rcv_n_fds = 0;

        for (size_t i = 0; i < self->_rcv_n_ch; i++) {
                self->_ch[i] = NULL;
        }
        self->_rcv_n_ch = 0;

        free(self->_ch);
        self->_ch = NULL;
}

int receiver_add(Receiver *self, Channel *ch)
{
        int fds, flags, status;

        /* Add channel to read fds set. */
        FD_SET(ch->ch_read, &self->_rcv_fds);
        fds = ch->ch_read + 1;
        if (fds > self->_rcv_n_fds) {
                self->_rcv_n_fds = fds + 1;
        }

        /* Set read end to non-blocking. */
        errno = 0;
        flags = fcntl(ch->ch_read, F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "Failed to add to Receiver: %s\n", strerror(errno));
                return -1;
        }
        flags |= O_NONBLOCK;

        errno = 0;
        status = fcntl(ch->ch_read, F_SETFL, flags);
        if (status == -1) {
                fprintf(stderr, "Failed to add to Receiver: %s\n", strerror(errno));
                return -1;
        }

        /* Add channel to channel list. */
        if (self->_rcv_n_ch >= self->_rcv_max_ch) {
                fprintf(stderr, "Failed to add to Receiver: Receiver is full\n");
                return -1;
        }

        self->_ch[self->_rcv_n_ch++] = ch;

        return 0;
}

int receiver_cb_sigchld(struct Channel ch)
{
        SigchldDTO dto;

        for (;;) {
                /* Drain pipe and for any SIGCHLD DTOs. */
                errno = 0;
                if (read(ch.ch_read, &dto, sizeof(dto)) == -1) {
                        if (errno == EAGAIN) {
                                break;
                        } else {
                                fprintf(stderr, "Failed to receive data: %s\n",
                                        strerror(errno));
                                return -1;
                        }
                }

                /* Update relevant job in job table. */
                job_table.update(&job_table, dto.chld_pid, dto.chld_status);
        }

        return 0;
}

int receiver_consume_events(Receiver *self)
{
        int ready, n_fds;
        struct timeval timeout;
        fd_set fds;
        Channel ch;

        /*
         * Don't block; just poll for file descriptors.
         *
         * Source: TLPI section 63.2.1
         */
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        n_fds = self->_rcv_n_fds;
        fds = self->_rcv_fds;

        errno = 0;
        while ((ready = select(n_fds, &fds, NULL, NULL, &timeout)) == -1 && errno == EINTR)
                ;
        if (ready == -1 && errno != EINTR) {
                fprintf(stderr, "Failed to consume events: %s\n", strerror(errno));
                return -1;
        }

        for (size_t i = 0; i < self->_rcv_n_ch; i++) {
                ch = *self->_ch[i];
                if (FD_ISSET(ch.ch_read, &fds)) {
                        ch.ch_callback(ch);
                }
        }

        FD_ZERO(&fds);
        for (size_t i = 0; i < self->_rcv_n_ch; i++) {
                ch = *self->_ch[i];
                FD_SET(ch.ch_read, &fds);
        }

        return 0;
}
