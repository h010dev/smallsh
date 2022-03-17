/**
 * @file SH_cd.c
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief SH_cd builtin command.
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
void SH_cd(char const * const dirname)
{
        int status;
        char const *dir;

        /* Default to $HOME directory if dirname not supplied. */
        dir = dirname;
        if (dir == NULL) {
                dir = getenv("HOME");
                if (dir == NULL) {
                        /* $HOME env var not defined. */
                        fprintf(stderr, "$HOME: %s\n", strerror(errno));
                        fflush(stderr);
                        return;
                }
        }

        errno = 0;
        status = chdir(dir);
        if (status == -1) {
                /* Couldn't cd into directory. */
                fprintf(stderr, "-smallsh: cd: %s: %s\n", dir, strerror(errno));
                fflush(stderr);
        }
}
