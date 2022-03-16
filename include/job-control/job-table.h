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

/**
 * @brief JobTable object.
 */
typedef struct {
        size_t n_jobs; /**< number of jobs in table */
        SH_Job *head; /**< pointer to linked list head */
} SH_JobTable;

/**
 * @brief Initializes new JobTable object.
 * @return new JobTable object
 */
SH_JobTable *SH_CreateJobTable(void);

/**
 * @brief De-initializes self and frees up its resources.
 * @param table @c JobTable object to de-initialize
 */
void SH_DestroyJobTable(SH_JobTable *table);

/**
 * @brief Add a Job to the JobTable.
 * @param table JobTable object
 * @param job Job object
 */
void SH_JobTableAddJob(SH_JobTable *table, SH_Job *job);

/**
 * @brief Clean the JobTable, displaying completed job statuses along
 * the way.
 * @param table JobTable object
 */
void SH_JobTableCleanJobs(SH_JobTable *table);

/**
 * @brief Find a Job within the JobTable that has a matching @p job_pgid.
 * @param table JobTable object
 * @param job_pgid Job PGID
 * @return @c Job object if found, @c NULL if not
 */
SH_Job *SH_JobTableFindJob(SH_JobTable const *table, pid_t job_pgid);

/**
 * @brief Kills all children in JobTable.
 *
 * Called on program exit to clean up any unterminated children
 * the shell created.
 * @param table JobTable object
 */
void SH_JobTableKillAllJobs(SH_JobTable *table);

/**
 * @brief List all Jobs in a pretty-printed format.
 * @param table JobTable object
 */
void SH_JobTablePrintJobs(SH_JobTable const *table);

/**
 * @brief Update a Job's SH_status within the JobTable.
 * @param table JobTable object
 * @param pid Job PID
 * @param status SH_status to give Job
 * @return 0 if Job was found and updated, -1 otherwise
 */
int SH_JobTableUpdateJob(SH_JobTable const *table, pid_t pid, int status);

#endif //SMALLSH_JOB_TABLE_H
