/**
 * @file receiver.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Responsible for receiving and handling signal-generated messages sent
 * via a dedicated Channel.
 */
#ifndef SMALLSH_RECEIVER_H
#define SMALLSH_RECEIVER_H

#include <sys/select.h>

#include "channel.h"

/**
 * @brief A @c Receiver object consists of an array of @c Channels that it is
 * responsible for monitor for and respond to new signal-generated events.
 */
typedef struct {
        int _rcv_n_fds; /**< maximum file descriptor value for usage with select */
        fd_set _rcv_fds; /**< set of read file descriptors for usage with select */
        size_t _rcv_n_ch; /**< number of channels that receiver will monitor */
        size_t _rcv_max_ch; /**< maximum number of channels possible */
        Channel **_ch; /**< list of channels to monitor */
} Receiver;

/**
 * @brief Adds a new @c Channel to its channel list, and initializes its read
 * end.
 *
 * Behind the scenes, this function initializes the channel pipe's read end and
 * sets it to be non-blocking as per the self-pipe trick requirements. It also
 * updates the necessary data structures use in the select call for consuming
 * new events.
 * @param self @c Receiver to add channel to
 * @param ch @c Channel to add to receiver
 * @return 0 on success, -1 on failure
 */
int receiver_add(Receiver *self, Channel *ch);

/**
 * @brief Callback handler responsible for consuming and responding to SIGCHLD
 * events.
 *
 * This function will read all events from channel pipe, then update the global
 * job table with information received on newly completed child processes.
 * @param ch @c Channel to update
 * @return 0 on success, -1 on failure
 */
int receiver_cb_sigchld(struct Channel ch);

/**
 * @brief Consumes events for all of the channels in its notification list,
 * calling their respective callback handlers on receipt of relevant data.
 *
 * Behind the scenes, this function makes use of the select call to poll any
 * changes made to @p self's file descriptor read list. After responding
 * to the events, it clears out and resets its file descriptor read set.
 * This step is necessary in implementing the self-pipe trick.
 * @param self @c Receiver object
 * @return 0 on success, -1 on failure
 */
int receiver_consume_events(Receiver *self);

/**
 * @brief Initializes @p self with provided data.
 * @param self @c receiver to initialize
 * @param max_ch maximum number of channels receiver will support
 * @return 0 on success, -1 on failure
 */
int receiver_ctor(Receiver *self, size_t max_ch);

/**
 * @brief Resets @p self's values.
 * @param self @c Receiver to reset
 * @note This does not clear the actual channels by calling their respective
 * destructors. It is the caller's responsibility to do so afterwards.
 */
void receiver_dtor(Receiver *self);

#endif //SMALLSH_RECEIVER_H
