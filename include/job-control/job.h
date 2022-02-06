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

typedef struct {
        char *job_cmd;
        Process job_proc;
        pid_t job_pgid;
        struct termios tmodes;
        char *job_stdin;
        char *job_stdout;
        unsigned job_spec;
} Job;

#endif //SMALLSH_JOB_H
