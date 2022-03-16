/**
 * @file globals.c
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Shell global variables.
 */
#ifndef SMALLSH_GLOBALS_H
#define SMALLSH_GLOBALS_H

#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>

extern volatile sig_atomic_t smallsh_fg_only_mode_flag; /**< foreground-only flag for handlers */
extern int smallsh_fg_only_mode; /**< foreground-only mode */

extern int smallsh_interactive_mode; /**< whether or not shell is in interactive mode */
extern bool smallsh_line_buffer; /**< whether or not to add newlines to shell commands */
extern pid_t smallsh_shell_pgid; /**< shell's PGID */
extern int smallsh_shell_terminal; /**< shell's terminal file */

#endif //SMALLSH_GLOBALS_H
