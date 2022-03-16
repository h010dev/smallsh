/**
 * @file handler.h
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief Contains functions for handling signals.
 */
#ifndef SMALLSH_HANDLER_H
#define SMALLSH_HANDLER_H

/**
 * @brief Handles a SIGTSTP signal when fg_only_mode is enabled.
 *
 * Responds to a SIGTSTP event then calls @c handler_switch_disable_fg_only_mode()
 * so that the next SIGTSTP event undoes its action.
 *
 * Source: https://edstem.org/us/courses/16718/discussion/1067170
 * @param sig signal number
 */
void SH_HandlerDisableFgOnlyMode(int sig);

/**
 * @brief Handles a SIGTSTP signal when fg_only_mode is disabled.
 *
 * Responds to a SIGTSTP event then calls @c SH_HandlerSwitchEnableFgOnlyMode()
 * so that the next SIGTSTP event undoes its action.
 *
 * Source: https://edstem.org/us/courses/16718/discussion/1067170
 * @param sig signal number
 */
void SH_HandlerEnableFgOnlyMode(int sig);

/**
 * @brief Handles a SIGCHLD signal.
 *
 * Makes use of self-pipe trick described in TLPI section 63.5.2 in order to
 * send SIGCHLD information over a pipe, where the job table will consume
 * the data and update its internal state.
 *
 * @param sig signal number
 */
void SH_HandlerHandleSigchld(int sig);

/**
 * @brief Switches SIGTSTP mask to enable fg_only_mode on next receipt.
 *
 * Source: https://edstem.org/us/courses/16718/discussion/1067170
 */
void SH_HandlerSwitchEnableFgOnlyMode(void);

#endif //SMALLSH_HANDLER_H
