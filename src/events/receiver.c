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
SH_Receiver *SH_CreateReceiver(size_t max_channels)
{
        SH_Receiver *receiver;
        fd_set read_fds;

        receiver = malloc(sizeof *receiver);
        if (receiver == NULL) {
                fprintf(stderr, "malloc\n");
                return NULL;
        }

        receiver->_n_fds = 0;
        receiver->_n_channels = 0;
        receiver->_max_channels = max_channels;

        /* Init Channel array. */
        receiver->_channels = malloc(max_channels * sizeof(SH_Channel));
        if (receiver->_channels == NULL) {
                fprintf(stderr, "Failed to init Receiver: malloc()\n");
                return NULL;
        }

        for (size_t i = 0; i < max_channels; i++) {
                receiver->_channels[i] = NULL;
        }

        /* Zero-out read fds set */
        FD_ZERO(&read_fds);
        receiver->_fds = read_fds;

        return receiver;
}

void SH_DestroyReceiver(SH_Receiver *receiver)
{
        receiver->_n_fds = 0;

        for (size_t i = 0; i < receiver->_n_channels; i++) {
                receiver->_channels[i] = NULL;
        }
        receiver->_n_channels = 0;

        free(receiver->_channels);
        receiver->_channels = NULL;

        free(receiver);
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
int SH_ReceiverAddChannel(SH_Receiver *receiver, SH_Channel *channel)
{
        int fds, flags, status;

        /* Add channel to read fds set. */
        FD_SET(channel->read_fd, &receiver->_fds);
        fds = channel->read_fd + 1;
        if (fds > receiver->_n_fds) {
                receiver->_n_fds = fds + 1;
        }

        /* Set read end to non-blocking. */
        errno = 0;
        flags = fcntl(channel->read_fd, F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "Failed to add to Receiver: %s\n", strerror(errno));
                return -1;
        }
        flags |= O_NONBLOCK;

        errno = 0;
        status = fcntl(channel->read_fd, F_SETFL, flags);
        if (status == -1) {
                fprintf(stderr, "Failed to add to Receiver: %s\n", strerror(errno));
                return -1;
        }

        /* Add channel to channel list. */
        if (receiver->_n_channels >= receiver->_max_channels) {
                fprintf(stderr, "Failed to add to Receiver: Receiver is full\n");
                return -1;
        }

        receiver->_channels[receiver->_n_channels++] = channel;

        return 0;
}

int SH_ReceiverSigchldCallbackHandler(struct SH_Channel channel)
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

int SH_ReceiverConsumeEvents(SH_Receiver *receiver)
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

        n_fds = receiver->_n_fds;
        fds = receiver->_fds;

        errno = 0;
        while ((ready = select(n_fds, &fds, NULL, NULL, &timeout)) == -1 && errno == EINTR)
                ;
        if (ready == -1 && errno != EINTR) {
                fprintf(stderr, "Failed to consume events: %s\n", strerror(errno));
                return -1;
        }

        for (size_t i = 0; i < receiver->_n_channels; i++) {
                ch = *receiver->_channels[i];
                if (FD_ISSET(ch.read_fd, &fds)) {
                        ch.callback_handler(ch);
                }
        }

        FD_ZERO(&fds);
        for (size_t i = 0; i < receiver->_n_channels; i++) {
                ch = *receiver->_channels[i];
                FD_SET(ch.read_fd, &fds);
        }

        return 0;
}
