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
SH_Sender *SH_CreateSender(size_t const capacity)
{
        SH_Sender *sender;

        sender = malloc(sizeof *sender);
        if (sender == NULL) {
                fprintf(stderr, "malloc\n");
                return NULL;
        }

        sender->size = 0;
        sender->capacity = capacity;

        /* Init Channel array. */
        sender->channels = malloc(capacity * sizeof(SH_Channel));
        if (sender->channels == NULL) {
                fprintf(stderr, "Failed to init Sender: malloc()\n");
                free(sender);
                return NULL;
        }

        for (size_t i = 0; i < capacity; i++) {
                sender->channels[i] = NULL;
        }

        return sender;
}

void SH_DestroySender(SH_Sender **sender)
{
        for (size_t i = 0; i < (*sender)->size; i++) {
                (*sender)->channels[i] = NULL;
        }
        (*sender)->size = 0;

        free((*sender)->channels);
        (*sender)->channels = NULL;

        free(*sender);
        *sender = NULL;
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
int SH_SenderAddChannel(SH_Sender * const sender, SH_Channel * const channel)
{
        int flags, status;

        /* Set write end to non-blocking. */
        errno = 0;
        flags = fcntl(channel->write_fd, F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "Failed to add to Sender: %s\n",
                        strerror(errno));
                return -1;
        }
        flags |= O_NONBLOCK;

        errno = 0;
        status = fcntl(channel->write_fd, F_SETFL, flags);
        if (status == -1) {
                fprintf(stderr, "Failed to add to Sender: %s\n",
                        strerror(errno));
                return -1;
        }

        /* Add channel to channel list. */
        if (sender->size >= sender->capacity) {
                fprintf(stderr, "Failed to add to Sender: Sender is full\n");
                return -1;
        }

        sender->channels[sender->size++] = channel;

        return 0;
}

int SH_SenderNotifySigchldEvent(SH_Channel * const channel)
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
                if (write(channel->write_fd, &dto, sizeof(dto)) == -1
                        && errno != EAGAIN) {
                        return -1;
                }
        }
        if (child_pid == -1 && errno != ECHILD) {
                return -1;
        }

        return 0;
}
