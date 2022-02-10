//
// Created by mohamed on 2/9/22.
//

#ifndef SMALLSH_GLOBALS_H
#define SMALLSH_GLOBALS_H

#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>

int smallsh_fg_only_mode; /**< foreground-only mode */
volatile sig_atomic_t fg_flag; /**< foreground-only mode */
int shell_is_interactive; /**< whether or not shell is in interactive mode */
pid_t shell_pgid; /**< shell's PGID */
int shell_terminal; /**< shell's terminal file */
int smallsh_status;

#endif //SMALLSH_GLOBALS_H
