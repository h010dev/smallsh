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

#include <termios.h>
#include "process.h"

typedef struct Job Job;

/**
 * @brief Job object.
 */
struct Job {
        Process *job_proc; /**< process object */
        pid_t job_pgid; /**< PGID */
        char *job_stdin; /**< STDIN filename */
        char *job_stdout; /**< STDOUT filename */
        unsigned job_spec; /**< position within job table */
        bool job_bg; /**< whether or not job is to run in background */
        Job *job_next; /**< next job in table */
};

/**
 * @brief Initializes values for a @c Job object.
 * @param self job object to initialize
 * @param proc job's process object
 * @param pgid job's PGID
 * @param infile job's STDIN filename
 * @param outfile job's STDOUT filename
 * @param bg whether or not job is to be run in background
 */
void job_ctor(Job *self, Process *proc, pid_t pgid, char *infile, char *outfile,
              bool bg);

/**
 * @brief Cleans up and frees job resources.
 * @param self job to clean up
 */
void job_dtor(Job *self);

#endif //SMALLSH_JOB_H
