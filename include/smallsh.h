/**
 * @file smallsh.c
 * @author Mohamed Al-Hussein
 * @date 01 Feb 2022
 * @brief Contains core shell functions.
 *
 * Some of the ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
#ifndef SMALLSH_H
#define SMALLSH_H

/**
 * @brief Run shell's main event loop until terminated.
 */
int smallsh_run(void);

#endif //SMALLSH_H
