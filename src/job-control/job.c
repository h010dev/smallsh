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
void job_ctor(Job *self, char *cmd, Process *proc, pid_t pgid, char *infile,
              char *outfile, bool bg)
{
        /* Copy job command. */
        self->job_cmd = strdup(cmd);
        if (self->job_cmd == NULL) {
                fprintf(stderr, "strdup\n");
                exit(1);
        }

        /* Initialize job variables */
        self->job_proc = proc;
        self->job_pgid = pgid;
        self->job_bg = bg;

        /* Next job is null (for use with job table). */
        self->job_next = NULL;

        /* Copy input/output file names. */
        /* Case 1: Filename is a string. */
        if (infile != NULL) {
                self->job_stdin = strdup(infile);
                if (self->job_stdin == NULL) {
                        fprintf(stderr, "strdup\n");
                        exit(1);
                }
        }
        /* Case 2: Filename is null. */
        else {
                self->job_stdin = NULL;
        }

        /* Case 1: Filename is a string. */
        if (outfile != NULL) {
                self->job_stdout = strdup(outfile);
                if (self->job_stdout == NULL) {
                        fprintf(stderr, "strdup\n");
                        exit(1);
                }
        }
        /* Case 2: Filename is null. */
        else {
                self->job_stdout = NULL;
        }

}

void job_dtor(Job *self)
{
        /* Free job command. */
        free(self->job_cmd);

        /* Free process object. */
        process_dtor(self->job_proc);
        free(self->job_proc);
        self->job_proc = NULL;

        /* Clear variables. */
        self->job_pgid = 0;
        self->job_bg = false;
        self->job_next = NULL;

        /* Free input/output file names. */
        free(self->job_stdin);
        self->job_stdin = NULL;

        free(self->job_stdout);
        self->job_stdout = NULL;
}
