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

extern JobTable job_table; /**< shell global job-control table */

/**
 * @brief Creates new child process and runs @p job within child.
 * @param job job to run
 * @param foreground whether or not the job should run in the foreground
 * @return 0 on success, -1 on failure
 */
int job_control_launch_job(Job **job, bool foreground);

#endif //SMALLSH_JOB_CONTROL_H
