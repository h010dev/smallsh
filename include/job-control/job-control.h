/**
 * @date 05 Feb 2022
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
void job_control_foreground_job(Job *job, int cont);

/* Put a job in the background. If the cont argument is true, send the process
 * group a SIGCONT signal to wake it up. */
void job_control_background_job(Job *job, int cont);

/* Source: https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html */
void job_control_launch_job(Job **job, bool foreground);

#endif //SMALLSH_JOB_CONTROL_H
