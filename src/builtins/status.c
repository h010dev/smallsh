/**
 * @file status.c
 * @author Mohamed Al-Hussein
 * @date 03 Feb 2022
 * @brief status builtin command.
 */
#include <stdio.h>

#include "builtins/status.h"
#include "error.h"
#include "globals.h"

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
void status()
{
        if (!smallsh_interactive_mode) {
                fprintf(stdout, "\nexit value %d\n", smallsh_errno);
        } else {
                fprintf(stdout, "exit value %d\n", smallsh_errno);
        }

        fflush(stdout);
}
