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
void SH_exit(int const status)
{
        /* Clean up job table and kill any child processes. */
        SH_JobTableKillAllJobs(job_table);
        SH_DestroyJobTable(job_table);
        job_table = NULL;

        /* Teardown event handling channels. */
        SH_CleanupEvents();

        /* Make exit output pretty in case we are operating inside another shell. */
        if (!smallsh_interactive_mode) {
                write(STDOUT_FILENO, "\n", 1);
        }
        write(STDOUT_FILENO, "exit\n", 5);

        exit(status);
}
