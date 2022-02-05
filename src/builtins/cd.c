/**
 * @file cd.c
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief cd builtin command.
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins/cd.h"

void cd(char *dirname)
{
        int status;
        char *msg;

        /* default to home directory if dirname not supplied */
        if (dirname == NULL) {
                dirname = getenv("HOME");
                if (dirname == NULL) {
                        /* no match found! error */
                        msg = "$HOME is not defined!\n";
                        write(STDERR_FILENO, msg, strlen(msg));
                        return;
                }
        }

        errno = 0; /* chdir sets errno on failure */
        status = chdir(dirname);
        if (status == -1) {
                msg = strerror(errno);
                write(STDERR_FILENO, msg, strlen(msg));
        }
}
