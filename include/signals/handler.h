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

// 34.6.1
void dispatcher_dispatch_sighup(pid_t pid);
void handler_handle_sighup(int sig);
void handler_handle_sigint(int sig);

#endif //SMALLSH_HANDLER_H
