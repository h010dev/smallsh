/**
 * @file job-control.h
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Functions for performing shell job control.
 *
 * Ideas presented here were retrieved from the following sources:
 * https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 * https://www.gnu.org/software/libc/manual/html_node/Freground-and-Background.html
 * https://www.gnu.org/software/libc/manual/html_node/Stopped-and-Terminated-Jobs.html
 */
#ifndef SMALLSH_JOB_CONTROL_H
#define SMALLSH_JOB_CONTROL_H

#include "job.h"
#include "job-table.h"

SH_JobTable *job_table; /**< shell global job-control table */

/**
 * @brief Creates new child process and runs @p job within child.
 * @param job job to run
 * @param run_fg whether or not the job should run in the foreground
 * @return 0 on success, -1 on failure
 */
int SH_JobControlLaunchJob(SH_Job **job, bool run_fg);

#endif //SMALLSH_JOB_CONTROL_H
