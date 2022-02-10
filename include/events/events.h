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

Channel ch_sigchld; /**< communication channel for SIGCHLD events */
Channel ch_sigtstp; /**< communication channel for SIGTSTP events */
Receiver rcv; /**< list of channels waiting on new events */
Sender snd; /**< list of channels to notify on new events */

/**
 * @brief Cleans up the global event-related data structures.
 */
void events_cleanup(void);

 /**
  * @brief Initializes global event-related data structures.
  * @return 0 on success, -1 on failure
  */
int events_init(void);

/**
 * @brief Consumes new events, notifies user of any them, and removes completed
 * jobs from global job table.
 */
int events_notify(void);

#endif //SMALLSH_EVENTS_H
