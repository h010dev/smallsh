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
typedef struct JobTable JobTable;

/**
 * @brief JobTable object.
 */
struct JobTable {
        /**
         * @brief Add a job to the table.
         * @param self table to add job to
         * @param job job to add to table
         */
        void (*add_job) (struct JobTable const * const self, Job *job);

        /**
         * @brief Clean the job table, displaying completed job statuses along
         * the way.
         * @param self table to clean
         */
        void (*clean) (struct JobTable const * const self);

        /**
         * @brief Find a job within the table that has a matching @p job_pgid.
         * @param self table to search for job in
         * @param job_pgid job PGID to search for
         * @return @c Job object if found, @c NULL if not
         */
        Job *(*find_job) (struct JobTable const * const self, pid_t job_pgid);

        void (*killall) (struct JobTable const * const self);

        /**
         * @brief List all jobs in a pretty-printed format.
         * @param self table to print
         */
        void (*list_jobs) (struct JobTable const * const self);

        /**
         * @brief Update a job's status within the table.
         * @param self table where job resides
         * @param pid job's PID
         * @param status status to give job
         * @return 0 if job was found and updated, -1 otherwise
         */
        int (*update) (struct JobTable const * const self, pid_t pid, int status);

        /**
         * @brief Contains internal data for table.
         */
        struct JobTablePrivate *private;
};

void job_table_ctor(JobTable *self);
void job_table_dtor(JobTable *self);

#endif //SMALLSH_JOB_TABLE_H
