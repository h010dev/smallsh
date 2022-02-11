/**
 * @file channel.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Provides means for sending and receiving signal-related information
 * via a message channel.
 */
#include <errno.h>
#include <stdio.h>
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
int channel_ctor(Channel *self, int (*cb_handler) (struct Channel))
{
        int pipe_[2];

        self->ch_callback = cb_handler;

        errno = 0;
        if (pipe(pipe_) == -1) {
                fprintf(stderr, "Failed to init Channel: %s\n", strerror(errno));
                return -1;
        }

        self->ch_read = pipe_[0];
        self->ch_write = pipe_[1];

        return 0;
}

void channel_dtor(Channel *self)
{
        self->ch_read = 0;
        self->ch_write = 0;
        self->ch_callback = NULL;
}
