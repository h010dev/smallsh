/**
 * @file events.h
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Contains functions for handling signal-generated events.
 */
#ifndef SMALLSH_EVENTS_H
#define SMALLSH_EVENTS_H

#include "channel.h"
#include "receiver.h"
#include "sender.h"

extern Channel ch_sigchld; /**< communication channel for SIGCHLD events */
extern Receiver rcv; /**< list of channels waiting on new events */
extern Sender snd; /**< list of channels to notify on new events */

/**
 * @brief Cleans up the global event-related data structures.
 */
void SH_CleanupEvents(void);

 /**
  * @brief Initializes global event-related data structures.
  * @return 0 on success, -1 on failure
  */
int SH_InitEvents(void);

/**
 * @brief Consumes new events, notifies user of any them, and removes completed
 * jobs from global job table.
 */
int SH_NotifyEvents(void);

#endif //SMALLSH_EVENTS_H
