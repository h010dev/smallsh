/**
 * @file channel.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Provides means for sending and receiving signal-related information
 * via a message channel.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "events/channel.h"

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
SH_Channel *SH_CreateChannel(int (*cb_handler) (SH_Channel))
{
        SH_Channel *channel;
        int pipe_[2];

        channel = malloc(sizeof *channel);
        if (channel == NULL) {
                fprintf(stderr, "malloc\n");
                return NULL;
        }

        channel->callback_handler = cb_handler;

        errno = 0;
        if (pipe(pipe_) == -1) {
                fprintf(stderr, "Failed to init Channel: %s\n", strerror(errno));
                return NULL;
        }

        channel->read_fd = pipe_[0];
        channel->write_fd = pipe_[1];

        return channel;
}

void SH_DestroyChannel(SH_Channel *channel)
{
        channel->read_fd = 0;
        channel->write_fd = 0;
        channel->callback_handler = NULL;

        free(channel);
}
