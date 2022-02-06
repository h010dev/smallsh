/**
 * @file job-table.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a collection of jobs.
 *
 * Ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Data-Structures.html
 */
#include <stdlib.h>

#include "job-control/job-table.h"

struct JobTablePrivate {
        Job **jt_table; /**< job array */
        size_t jt_table_size; /**< job table size */
        size_t jt_job_count; /**< number of jobs in table */
        Job *jt_last_job; /**< most recently added job */
};

int job_table_clean_(struct JobTable const * const self)
{
        /**
         * Remove completed jobs from table.
         */
        (void) self;
        return 0;
}

int job_table_update_(struct JobTable const * const self, pid_t pid, int status)
{
        struct JobTablePrivate *priv = self->private;

        /**
         * Find and remove job containing process with matching pid.
         */
         Job *job;
         for (size_t i = 0; i < priv->jt_table_size; i++) {
                 job = priv->jt_table[i];
                 if (job != NULL && job->job_proc.proc_pid == pid) {
                         break;
                 }
         }

         /**
          * Error! Job not found.
          */
          if (job == NULL) {
                  return -1;
          }

          /**
           * Update job status.
           */
          job->job_proc.proc_status = status;

          return 0;
}

int job_table_resize_(struct JobTable const * const self, size_t new_size)
{
        struct JobTablePrivate *priv = self->private;

        Job **tmp = realloc(priv->jt_table, new_size * sizeof(Job *));
        if (tmp == NULL) {
                return -1;
        }
        priv->jt_table = tmp;

        return 0;
}

int job_table_add_job_(struct JobTable const * const self, Job *job)
{
        struct JobTablePrivate *priv = self->private;

        /**
         * Resize table if full.
         */
        if (priv->jt_job_count >= priv->jt_table_size) {
                job_table_resize_(self, priv->jt_table_size * 2);
        }

        /**
         * Find a free slot in table and insert job there.
         */
        for (size_t i = 0; i < priv->jt_table_size; i++) {
                if (priv->jt_table[i] == NULL) {
                        priv->jt_table[i] = job;
                        break;
                }
        }

        /**
         * Update job spec number for new job.
         */
        unsigned job_spec;
        if (priv->jt_last_job != NULL) {
                job_spec = priv->jt_last_job->job_spec + 1;
        } else {
                job_spec = 1;
        }
        job->job_spec = job_spec;

        /**
         * Update last job to point to new job.
         */
        priv->jt_last_job = job;

        return 0;
}

Job *job_table_find_job_(struct JobTable const * const self, pid_t job_pgid)
{
        struct JobTablePrivate *priv = self->private;

        /**
         * Search all entries for job with matching pgid.
         */
        for (size_t i = 0; i < priv->jt_table_size; i++) {
                Job *job = priv->jt_table[i];
                if (job != NULL && job->job_pgid == job_pgid) {
                        return job;
                }
        }

        return NULL; /* job not found */
}

void job_table_list_jobs_(struct JobTable const * const self)
{
        (void) self;
}

void job_table_ctor(JobTable *self)
{
        /**
         * Init table functions.
         */
        self->add_job = &job_table_add_job_;
        self->find_job = &job_table_find_job_;
        self->list_jobs = &job_table_list_jobs_;
        self->update = &job_table_update_;

        /**
         * Init table data.
         */
        self->private = malloc(sizeof(struct JobTablePrivate));
        self->private->jt_table = malloc(sizeof(Job *));
        self->private->jt_table_size = 1;
        self->private->jt_job_count = 0;
        self->private->jt_last_job = 0;
}

void job_table_dtor(JobTable *self)
{
        (void) self;
}
