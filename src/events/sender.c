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
SH_Sender *SH_CreateSender(size_t max_channels)
{
        SH_Sender *sender;

        sender = malloc(sizeof *sender);
        if (sender == NULL) {
                fprintf(stderr, "malloc\n");
                return NULL;
        }

        sender->_n_channels = 0;
        sender->_max_channels = max_channels;

        /* Init Channel array. */
        sender->_channels = malloc(max_channels * sizeof(SH_Channel));
        if (sender->_channels == NULL) {
                fprintf(stderr, "Failed to init Sender: malloc()\n");
                return NULL;
        }

        for (size_t i = 0; i < max_channels; i++) {
                sender->_channels[i] = NULL;
        }

        return sender;
}

void SH_DestroySender(SH_Sender *sender)
{
        for (size_t i = 0; i < sender->_n_channels; i++) {
                sender->_channels[i] = NULL;
        }
        sender->_n_channels = 0;

        free(sender->_channels);
        sender->_channels = NULL;

        free(sender);
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
int SH_SenderAddChannel(SH_Sender *sender, SH_Channel *channel)
{
        int flags, status;

        /* Set write end to non-blocking. */
        errno = 0;
        flags = fcntl(channel->write_fd, F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "Failed to add to Sender: %s\n", strerror(errno));
                return -1;
        }
        flags |= O_NONBLOCK;

        errno = 0;
        status = fcntl(channel->write_fd, F_SETFL, flags);
        if (status == -1) {
                fprintf(stderr, "Failed to add to Sender: %s\n", strerror(errno));
                return -1;
        }

        /* Add channel to channel list. */
        if (sender->_n_channels >= sender->_max_channels) {
                fprintf(stderr, "Failed to add to Sender: Sender is full\n");
                return -1;
        }

        sender->_channels[sender->_n_channels++] = channel;

        return 0;
}

int SH_SenderNotifySigchldEvent(SH_Channel *channel)
{
        int status;
        pid_t child_pid;
        SH_SigchldDTO dto;

        /*
         * Catch all incoming SIGCHLD signals and dispatch their status to
         * listening parties.
         *
         * Source: TLPI section 26.3
         */
        errno = 0;
        while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
                /* Initialize DTO object to transfer PID and status through. */
                dto.pid = child_pid;
                dto.status = status;

                /* Write DTO to pipe. */
                errno = 0;
                if (write(channel->write_fd, &dto, sizeof(dto)) == -1 && errno != EAGAIN) {
                        return -1;
                }
        }
        if (child_pid == -1 && errno != ECHILD) {
                return -1;
        }

        return 0;
}
