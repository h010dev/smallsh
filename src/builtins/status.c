/**
 * @file SH_status.c
 * @author Mohamed Al-Hussein
 * @date 03 Feb 2022
 * @brief SH_status builtin command.
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
void SH_status()
{
        if (!smallsh_interactive_mode) {
                fprintf(stdout, "\nexit value %d\n", smallsh_errno);
        } else {
                fprintf(stdout, "exit value %d\n", smallsh_errno);
        }

        fflush(stdout);
}
