/**
 * @file shell-attrs.h
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contains functions for handling signals.
 *
 * Ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
#ifndef SMALLSH_SHELL_ATTRS_H
#define SMALLSH_SHELL_ATTRS_H

#include "job-control/job-table.h"

int shell_is_interactive; /**< whether or not shell is in interactive mode */
pid_t shell_pgid; /**< shell's PGID */
int shell_terminal; /**< shell's terminal file */
int pipe_sigchld[2]; /**< communication pipe for SIGCHLD events */
int pipe_sigtstp[2]; /**< communication pipe for SIGTSTP events */
int smallsh_status;
JobTable job_table;

#endif //SMALLSH_SHELL_ATTRS_H
