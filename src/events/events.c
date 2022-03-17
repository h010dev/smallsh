/**
 * @file events.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Contains functions for handling signal-generated events.
 */
#include <stdio.h>

#include "events/events.h"
#include "job-control/job-control.h"
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
 * MACROS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
#define SH_MAX_EVENTS 1
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
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
int SH_InitEvents(void)
{
        int status;

        /* Initialize event channels */
        sigchld_channel = SH_CreateChannel(SH_ReceiverSigchldCallbackHandler);
        if (sigchld_channel == NULL) {
                fprintf(stderr, "SH_CreateChannel()");
                return -1;
        }

        /* Initialize event handler */
        receiver = SH_CreateReceiver(SH_MAX_EVENTS);
        if (receiver == NULL) {
                fprintf(stderr, "SH_CreateReceiver()");
                return -1;
        }

        status = SH_ReceiverAddChannel(receiver, sigchld_channel);
        if (status == -1) {
                fprintf(stderr, "SH_ReceiverAddChannel()");
                return -1;
        }

        /* Initialize event notifier */
        sender = SH_CreateSender(SH_MAX_EVENTS);
        if (sender == NULL) {
                fprintf(stderr, "SH_CreateSender()");
                return -1;
        }

        status = SH_SenderAddChannel(sender, sigchld_channel);
        if (status == -1) {
                fprintf(stderr, "SH_SenderAddChannel()");
                return -1;
        }

        return 0;
}

void SH_CleanupEvents(void)
{
        SH_DestroyReceiver(&receiver);
        SH_DestroySender(&sender);
        SH_DestroyChannel(&sigchld_channel);
}

int SH_NotifyEvents(void)
{
        int status;

        status = SH_ReceiverConsumeEvents(receiver);
        if (status == -1) {
                fprintf(stderr, "SH_ReceiverConsumeEvents()\n");
                return -1;
        }

        SH_JobTableCleanJobs(job_table);

        return 0;
}
