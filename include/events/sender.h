/**
 * @file sender.h
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Responsible for sending messages to subscribers via a dedicated Channel.
 */
#ifndef SMALLSH_SENDER_H
#define SMALLSH_SENDER_H

#include "channel.h"

/**
 * @brief A @c Sender object consists of an array of @c Channels that it is
 * responsible for sending messages to on receipt of signal-generated events.
 */
typedef struct {
        size_t _snd_n_ch; /**< number of channels in notify list */
        size_t _snd_max_ch; /**< maximum number of channels possible */
        Channel **_ch; /**< list of channels no notify */
} Sender;

/**
 * @brief Adds a new @c Channel to its channel list, and initializes its write
 * end.
 *
 * Behind the scenes, this function initializes the channel pipe's write end
 * and sets it to be non-blocking as per the self-pipe trick requirements.
 * @param self @c Sender to add channel to
 * @param ch @c Channel to add to sender
 * @return 0 on success, -1 on failure
 */
int sender_add(Sender *self, Channel ch);

/**
 * @brief Initializes @p self with provided data.
 * @param self @c Sender to initialize
 * @param max_ch maximum number of channels sender will support
 * @return 0 on success, -1 on failure
 */
int sender_ctor(Sender *self, size_t max_ch);

/**
 * @brief Resets @p self's values.
 * @param self @c Sender to reset
 * @note This does not clear the actual channels by calling their respective
 * destructors. It is the caller's responsibility to do so afterwards.
 */
void sender_dtor(Sender *self);

/**
 * @brief Sends data received via a SIGCHLD signal to @p self.
 *
 * This function abstracts the signal-handling step used in the self-pipe trick
 * (TLPI section 63.5.2) in an attempt to relay SIGCHLD data while doing its
 * best at remaining async-safe.
 *
 * Only async-safe system calls are made within this function, and the only
 * global data type that is being accessed within is the pipe's write file
 * descriptor, which is done so for reads only, and without any dereferences.
 * The file descriptor is initialized on program startup, and remains unchanged
 * until program termination.
 *
 * The data sent is used for updating the shell's global job table, as it
 * requires information on both which child completed and what their exit
 * status was.
 * @param ch @c Channel to send data to
 * @return 0 on success, -1 on failure
 */
int sender_notify_sigchld(struct Channel ch);

/**
 * @brief Sends data received via a SIGTSTP signal to @p self.
 *
 * This function abstracts the signal-handling step used in the self-pipe trick
 * (TLPI section 63.5.2) in an attempt to relay SIGTSTP events while doing its
 * best at remaining async-safe.
 *
 * Only async-safe system calls are made within this function, and the only
 * global data type that is being accessed within is the pipe's write file
 * descriptor, which is done so for reads only, and without any dereferences.
 * The file descriptor is initialized on program startup, and remains unchanged
 * until program termination.
 *
 * The data sent is used to notify the shell that it should switch its
 * foreground-only mode on or off depending on its current state.
 * @param ch @c Channel to send data to
 * @return 0 on success, -1 on failure
 */
int sender_notify_sigtstp(struct Channel ch);

#endif //SMALLSH_SENDER_H
