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
SH_Job *SH_CreateJob(char *command, SH_Process *proc, char *infile,
                     char *outfile, bool run_bg)
{
        SH_Job *job;

        job = malloc(sizeof *job);
        if (job == NULL) {
                fprintf(stderr, "malloc\n");
                exit(1);
        }

        /* Copy job command. */
        job->command = strdup(command);
        if (job->command == NULL) {
                fprintf(stderr, "strdup\n");
                exit(1);
        }

        /* Initialize job variables */
        job->proc = proc;
        job->pgid = 0;
        job->run_bg = run_bg;

        /* Next job is null (for use with job table). */
        job->next = NULL;

        /* Copy input/output file names. */
        /* Case 1: Filename is a string. */
        if (infile != NULL) {
                job->infile = strdup(infile);
                if (job->infile == NULL) {
                        fprintf(stderr, "strdup\n");
                        exit(1);
                }
        }
        /* Case 2: Filename is null. */
        else {
                job->infile = NULL;
        }

        /* Case 1: Filename is a string. */
        if (outfile != NULL) {
                job->outfile = strdup(outfile);
                if (job->outfile == NULL) {
                        fprintf(stderr, "strdup\n");
                        exit(1);
                }
        }
        /* Case 2: Filename is null. */
        else {
                job->outfile = NULL;
        }

        return job;
}

void SH_DestroyJob(SH_Job *job)
{
        /* Free job command. */
        free(job->command);

        /* Free process object. */
        SH_DestroyProcess(job->proc);
        job->proc = NULL;

        /* Clear variables. */
        job->pgid = 0;
        job->run_bg = false;
        job->next = NULL;

        /* Free input/output file names. */
        free(job->infile);
        job->infile = NULL;

        free(job->outfile);
        job->outfile = NULL;

        free(job);
}
