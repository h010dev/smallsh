/**
 * @date 31 Jan 2022
 */
#ifndef SMALLSH_H
#define SMALLSH_H

#include <sys/types.h>
#include "interpreter/statement.h"
#include "interpreter/statement.h"
#include "job-control/job.h"

int smallsh_eval(char *cmd);
void smallsh_exec(Statement *stmt);
ssize_t smallsh_read_command(char **cmd);
void smallsh_redirect(char **stdin_streams, char **stdout_streams);
int smallsh_register_signals(void);
int smallsh_run(void);
int smallsh_eval2(char *cmd);

/*
 * Initialize shell.
 *
 * Source: https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */

/* make sure the shell is running interactively as the foreground job before
 * proceeding */
void smallsh_init(void);

#endif //SMALLSH_H
