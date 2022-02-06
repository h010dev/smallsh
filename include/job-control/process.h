/**
 * @file process.h
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a single child process.
 *
 * Ideas presented here were retrieved from the following sources:
 * https://www.gnu.org/software/libc/manual/html_node/Data-Structures.html
 * https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 */
#ifndef SMALLSH_PROCESS_H
#define SMALLSH_PROCESS_H

#include <stdbool.h>
#include <sys/types.h>

typedef struct {
        char **argv;
        pid_t proc_pid;
        bool proc_completed;
        int proc_status;
} Process;

/*
 * Launch process.
 *
 * Source: https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 */
void process_launch(Process *proc, pid_t pgid, char *infile, char *outfile,
                    bool foreground);

#endif //SMALLSH_PROCESS_H
