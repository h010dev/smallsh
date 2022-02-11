/**
 * @file cd.c
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief cd builtin command.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins/cd.h"

/* *****************************************************************************
 * PUBLIC DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void cd(char *dirname)
{
        int status;

        /* Default to $HOME directory if dirname not supplied. */
        if (dirname == NULL) {
                dirname = getenv("HOME");
                if (dirname == NULL) {
                        /* $HOME env var not defined. */
                        fprintf(stderr, "$HOME: %s\n", strerror(errno));
                        fflush(stderr);
                        return;
                }
        }

        errno = 0;
        status = chdir(dirname);
        if (status == -1) {
                /* Couldn't cd into directory. */
                fprintf(stderr, "-smallsh: cd: %s: %s\n", dirname, strerror(errno));
                fflush(stderr);
        }
}
