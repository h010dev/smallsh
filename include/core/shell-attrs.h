/**
 * @date 05 Feb 2022
 */
#ifndef SMALLSH_SHELL_ATTRS_H
#define SMALLSH_SHELL_ATTRS_H

/* Source: https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html */
#include <sys/types.h>
#include <termios.h>

/* keep track of attributes of the shell */
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

#endif //SMALLSH_SHELL_ATTRS_H
