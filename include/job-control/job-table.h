/**
 * @file job-table.h
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a collection of jobs.
 *
 * Ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Data-Structures.html
 */
#ifndef SMALLSH_JOB_TABLE_H
#define SMALLSH_JOB_TABLE_H

#include "job.h"

struct JobTablePrivate;

typedef struct JobTable {
        int (*add_job) (struct JobTable const * const self, Job *job);
        Job *(*find_job) (struct JobTable const * const self, pid_t job_pgid);
        void (*list_jobs) (struct JobTable const * const self);
        int (*update) (struct JobTable const * const self, pid_t pid, int status);
        struct JobTablePrivate *private;
} JobTable;

void job_table_ctor(JobTable *self);
void job_table_dtor(JobTable *self);

#endif //SMALLSH_JOB_TABLE_H
