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
SH_Receiver *SH_CreateReceiver(size_t const capacity)
{
        SH_Receiver *receiver;
        fd_set read_fds;

        receiver = malloc(sizeof *receiver);
        if (receiver == NULL) {
                fprintf(stderr, "malloc\n");
                return NULL;
        }

        receiver->n = 0;
        receiver->size = 0;
        receiver->capacity = capacity;

        /* Init Channel array. */
        receiver->channels = malloc(capacity * sizeof(SH_Channel));
        if (receiver->channels == NULL) {
                fprintf(stderr, "Failed to init Receiver: malloc()\n");
                free(receiver);
                return NULL;
        }

        for (size_t i = 0; i < capacity; i++) {
                receiver->channels[i] = NULL;
        }

        /* Zero-out read fds set */
        FD_ZERO(&read_fds);
        receiver->fds = read_fds;

        return receiver;
}

void SH_DestroyReceiver(SH_Receiver **receiver)
{
        (*receiver)->n = 0;

        for (size_t i = 0; i < (*receiver)->size; i++) {
                (*receiver)->channels[i] = NULL;
        }
        (*receiver)->size = 0;

        free((*receiver)->channels);
        (*receiver)->channels = NULL;

        free(*receiver);
        *receiver = NULL;
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
int
SH_ReceiverAddChannel(SH_Receiver * const receiver, SH_Channel * const channel)
{
        int fds, flags, status;

        /* Add channel to read fds set. */
        FD_SET(channel->read_fd, &receiver->fds);
        fds = channel->read_fd + 1;
        if (fds > receiver->n) {
                receiver->n = fds + 1;
        }

        /* Set read end to non-blocking. */
        errno = 0;
        flags = fcntl(channel->read_fd, F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "Failed to add to Receiver: %s\n",
                        strerror(errno));
                return -1;
        }
        flags |= O_NONBLOCK;

        errno = 0;
        status = fcntl(channel->read_fd, F_SETFL, flags);
        if (status == -1) {
                fprintf(stderr, "Failed to add to Receiver: %s\n",
                        strerror(errno));
                return -1;
        }

        /* Add channel to channel list. */
        if (receiver->size >= receiver->capacity) {
                fprintf(stderr, "Failed to add to Receiver: Receiver is full\n");
                return -1;
        }

        receiver->channels[receiver->size++] = channel;

        return 0;
}

int SH_ReceiverSigchldCallbackHandler(struct SH_Channel const channel)
{
        SH_SigchldDTO dto;

        for (;;) {
                /* Drain pipe and for any SIGCHLD DTOs. */
                errno = 0;
                if (read(channel.read_fd, &dto, sizeof(dto)) == -1) {
                        if (errno == EAGAIN) {
                                break;
                        } else {
                                fprintf(stderr, "Failed to receive data: %s\n",
                                        strerror(errno));
                                return -1;
                        }
                }

                /* Update relevant job in job table. */
                SH_JobTableUpdateJob(job_table, dto.pid, dto.status);
        }

        return 0;
}

int SH_ReceiverConsumeEvents(SH_Receiver * const receiver)
{
        int ready, n_fds;
        struct timeval timeout;
        fd_set fds;
        SH_Channel ch;

        /*
         * Don't block; just poll for file descriptors.
         *
         * Source: TLPI section 63.2.1
         */
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        n_fds = receiver->n;
        fds = receiver->fds;

        errno = 0;
        while ((ready = select(n_fds, &fds, NULL, NULL, &timeout)) == -1
                && errno == EINTR)
                ;
        if (ready == -1 && errno != EINTR) {
                fprintf(stderr, "Failed to consume events: %s\n",
                        strerror(errno));
                return -1;
        }

        for (size_t i = 0; i < receiver->size; i++) {
                ch = *receiver->channels[i];
                if (FD_ISSET(ch.read_fd, &fds)) {
                        ch.callback_handler(ch);
                }
        }

        FD_ZERO(&fds);
        for (size_t i = 0; i < receiver->size; i++) {
                ch = *receiver->channels[i];
                FD_SET(ch.read_fd, &fds);
        }

        return 0;
}
