/**
 * @file events.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Contains functions for handling signal-generated events.
 */
#include <stdio.h>

#include "events/events.h"
#include "job-control/job-control.h"

int events_init(void)
{
        int status;

        /* Initialize event channels */
        status = channel_ctor(&ch_sigchld, receiver_cb_sigchld);
        if (status == -1) {
                fprintf(stderr, "channel_ctor()");
                return -1;
        }

        /* Initialize event handler */
        status = receiver_ctor(&rcv, 2);
        if (status == -1) {
                fprintf(stderr, "receiver_ctor()");
                return -1;
        }

        status = receiver_add(&rcv, &ch_sigchld);
        if (status == -1) {
                fprintf(stderr, "receiver_add()");
                return -1;
        }

        /* Initialize event notifier */
        status = sender_ctor(&snd, 2);
        if (status == -1) {
                fprintf(stderr, "sender_ctor()");
                return -1;
        }

        status = sender_add(&snd, &ch_sigchld);
        if (status == -1) {
                fprintf(stderr, "sender_add()");
                return -1;
        }

        return 0;
}

void events_cleanup(void)
{
        receiver_dtor(&rcv);
        sender_dtor(&snd);

        channel_dtor(&ch_sigchld);
}

int events_notify(void)
{
        int status;

        status = receiver_consume_events(&rcv);
        if (status == -1) {
                fprintf(stderr, "receiver_consume_events()\n");
                return -1;
        }

        job_table.clean(&job_table);

        return 0;
}
