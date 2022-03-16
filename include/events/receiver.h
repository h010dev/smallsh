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
        int n; /**< maximum file descriptor value for usage with select */
        fd_set fds; /**< set of read file descriptors for usage with select */
        size_t size; /**< number of channels that receiver will monitor */
        size_t capacity; /**< maximum number of channels possible */
        SH_Channel **channels; /**< list of channels to monitor */
} SH_Receiver;

/**
 * @brief Adds a new @c Channel to its channel list, and initializes its read
 * end.
 *
 * Behind the scenes, this function initializes the channel pipe's read end and
 * sets it to be non-blocking as per the self-pipe trick requirements. It also
 * updates the necessary data structures use in the select call for consuming
 * new events.
 * @param receiver @c Receiver to add channel to
 * @param channel @c Channel to add to receiver
 * @return 0 on success, -1 on failure
 */
int SH_ReceiverAddChannel(SH_Receiver *receiver, SH_Channel *channel);

/**
 * @brief Callback handler responsible for consuming and responding to SIGCHLD
 * events.
 *
 * This function will read all events from channel pipe, then update the global
 * job table with information received on newly completed child processes.
 * @param channel @c Channel to update
 * @return 0 on success, -1 on failure
 */
int SH_ReceiverSigchldCallbackHandler(SH_Channel channel);

/**
 * @brief Consumes events for all of the channels in its notification list,
 * calling their respective callback handlers on receipt of relevant data.
 *
 * Behind the scenes, this function makes use of the select call to poll any
 * changes made to @p self's file descriptor read list. After responding
 * to the events, it clears out and resets its file descriptor read set.
 * This step is necessary in implementing the self-pipe trick.
 * @param receiver @c Receiver object
 * @return 0 on success, -1 on failure
 */
int SH_ReceiverConsumeEvents(SH_Receiver *receiver);

/**
 * @brief Initializes a new @c Receiver object.
 * @param capacity maximum number of channels receiver will support
 * @return new @c Receiver object on success, @c NULL on failure
 */
SH_Receiver *SH_CreateReceiver(size_t capacity);

/**
 * @brief Resets @p self's values.
 * @param receiver @c Receiver to reset
 * @note This does not clear the actual channels by calling their respective
 * destructors. It is the caller's responsibility to do so afterwards.
 */
void SH_DestroyReceiver(SH_Receiver **receiver);

#endif //SMALLSH_RECEIVER_H
