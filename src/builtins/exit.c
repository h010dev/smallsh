/**
 * @file exit.h
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief exit builtin command.
 */
#include <stdlib.h>
#include <unistd.h>

#include "builtins/exit.h"
#include "events/events.h"
#include "globals.h"
#include "job-control/job-control.h"

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
void exit_(int status)
{
        /* Clean up job table and kill any child processes. */
        job_table.killall(&job_table);
        job_table_dtor(&job_table);

        /* Teardown event handling channels. */
        events_cleanup();

        /* Make exit output pretty in case we are operating inside another shell. */
        if (!smallsh_interactive_mode) {
                write(STDOUT_FILENO, "\n", 1);
        }
        write(STDOUT_FILENO, "exit\n", 5);

        exit(status);
}
