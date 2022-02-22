/**
 * @file job.h
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a a single job.
 *
 * Ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Data-Structures.html
 */
#ifndef SMALLSH_JOB_H
#define SMALLSH_JOB_H

#include "process.h"

typedef struct SH_Job SH_Job;

/**
 * @brief Job object.
 */
struct SH_Job {
        char *command; /**< command typed by user for this job */
        SH_Process *proc; /**< process object */
        pid_t pgid; /**< PGID */
        char *infile; /**< STDIN filename */
        char *outfile; /**< STDOUT filename */
        unsigned spec; /**< position within job table */
        bool run_bg; /**< whether or not job is to run in background */
        SH_Job *next; /**< next job in table */
};

/**
 * @brief Initializes new Job object.
 * @param command job command entered by user
 * @param proc job's process object
 * @param infile job's STDIN filename
 * @param outfile job's STDOUT filename
 * @param run_bg whether or not job is to be run in background
 * @return new Job object
 */
SH_Job *SH_CreateJob(char *command, SH_Process *proc, char *infile,
                     char *outfile, bool run_bg);

/**
 * @brief Cleans up and frees job resources.
 * @param job job to clean up
 */
void SH_DestroyJob(SH_Job *job);

#endif //SMALLSH_JOB_H
