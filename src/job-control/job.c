/**
 * @file job.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a a single job.
 *
 * Ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Data-Structures.html
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "job-control/job.h"

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
 * CONSTRUCTORS + DESTRUCTORS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void job_ctor(Job *self, Process *proc, pid_t pgid, char *infile, char *outfile,
              bool bg)
{
        /*
         * Initialize job variables
         */
        self->job_proc = proc;
        self->job_pgid = pgid;
        self->job_bg = bg;

        /*
         * Next job is null (for use with job table).
         */
        self->job_next = NULL;

        /*
         * Copy input/output file names.
         */
        if (infile != NULL) {
                /*
                 * Case 1: Filename is a string.
                 */
                self->job_stdin = strdup(infile);
                if (self->job_stdin == NULL) {
                        fprintf(stderr, "strdup\n");
                        exit(1);
                }
        } else {
                /*
                 * Case 2: Filename is null.
                 */
                self->job_stdin = NULL;
        }

        if (outfile != NULL) {
                /*
                 * Case 1: Filename is a string.
                 */
                self->job_stdout = strdup(outfile);
                if (self->job_stdout == NULL) {
                        fprintf(stderr, "strdup\n");
                        exit(1);
                }
        } else {
                /*
                 * Case 2: Filename is null.
                 */
                self->job_stdout = NULL;
        }

}

void job_dtor(Job *self)
{
        /*
         * Clear variables.
         */
        self->job_proc = 0;
        self->job_pgid = 0;
        self->job_bg = false;
        self->job_next = NULL;

        /*
         * Free input/output file names.
         */
        free(self->job_stdin);
        self->job_stdin = NULL;

        free(self->job_stdout);
        self->job_stdout = NULL;
}
