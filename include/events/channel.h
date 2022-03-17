/**
 * @file channel.h
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Provides means for sending and receiving signal-related information
 * via a message channel.
 */
#ifndef SMALLSH_CHANNEL_H
#define SMALLSH_CHANNEL_H

typedef struct SH_Channel SH_Channel;

/**
 * @brief A @c Channel object consists of a read end and a write end, as well
 * as a callback handler function that is invoked when new data is received.
 */
struct SH_Channel {
        int read_fd; /**< read file descriptor */
        int write_fd; /**< write file descriptor */
        int (*callback_handler) (SH_Channel channel); /**< callback handler */
};

/**
 * @brief Initializes a new Channel object.
 *
 * Behind the scenes, this function creates a pipe and hooks into its read and
 * write ends.
 * @param cb_handler callback method for this channel
 * @return new @c Channel object on success, @c NULL on failure
 */
SH_Channel *SH_CreateChannel(int (*cb_handler) (SH_Channel));

/**
 * @brief Resets @p self's values.
 * @param channel @c Channel to reset
 */
void SH_DestroyChannel(SH_Channel **channel);

#endif //SMALLSH_CHANNEL_H
