/**
 * @file exit.h
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief exit builtin command.
 */
#ifndef SMALLSH_EXIT_H
#define SMALLSH_EXIT_H

/**
 * @brief Clean-up all child processes and exit shell.
 * @param status exit status
 */
void exit_(int status);

#endif //SMALLSH_EXIT_H
