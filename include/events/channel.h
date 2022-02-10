/**
 * @file channel.h
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Provides means for sending and receiving signal-related information
 * via a message channel.
 */
#ifndef SMALLSH_CHANNEL_H
#define SMALLSH_CHANNEL_H

/**
 * @brief A @c Channel object consists of a read end and a write end, as well
 * as a callback handler function that is invoked when new data is received.
 */
typedef struct Channel {
        int ch_read; /**< read file descriptor */
        int ch_write; /**< write file descriptor */
        int (*ch_callback) (struct Channel self); /**< callback handler */
} Channel;

/**
 * @brief Initializes a @p self with provided data.
 *
 * Behind the scenes, this function creates a pipe and hooks into its read and
 * write ends.
 * @param self @c Channel to initialize
 * @param cb_handler callback method for this channel
 * @return 0 on success, -1 on failure
 */
int channel_ctor(Channel *self, int (*cb_handler) (struct Channel));

/**
 * @brief Resets @p self's values.
 * @param self @c Channel to reset
 */
void channel_dtor(Channel *self);

#endif //SMALLSH_CHANNEL_H
