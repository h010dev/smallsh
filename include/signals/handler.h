/**
 * @file handler.h
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief Contains functions for handling signals.
 */
#ifndef SMALLSH_HANDLER_H
#define SMALLSH_HANDLER_H

/**
 * @brief Handles a SIGCHLD signal.
 * @param sig signal number
 */
void handler_handle_sigchld(int sig);

/**
 * @brief Handles a SIGTSTP signal.
 * @param sig signal number
 */
void handler_handle_sigtstp(int sig);

void handler_enable_fg_only_mode(int sig);

void handler_disable_fg_only_mode(int sig);

void handler_install_fg_only_mode(void);

void handler_uninstall_fg_only_mode(void);

#endif //SMALLSH_HANDLER_H
