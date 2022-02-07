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

/*
 * Foreground / background process.
 *
 * Source: https://www.gnu.org/software/libc/manual/html_node/Foreground-and-Background.html
 */
/* Put job j in the foreground. If cont is nonzero, restore the saved terminal
 * modes and send the process group a SIGCONT signal to wake it up before we
 * block.
 */

/**
 * @brief Run @p job in the foreground.
 *
 * Puts @p job into the foreground, waits for it to complete, then puts
 * shell back into foreground.
 * @param job job to run
 */
void job_control_foreground_job(Job *job);

/**
 * @brief Creates new child process and runs @p job within child.
 * @param job job to run
 * @param foreground whether or not the job should run in the foreground
 */
void job_control_launch_job(Job **job, bool foreground);

#endif //SMALLSH_JOB_CONTROL_H
