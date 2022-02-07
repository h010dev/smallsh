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

/**
 * @brief A Process object holds information related to running a program.
 */
typedef struct {
        char **argv; /**< process arguments */
        pid_t proc_pid; /**< process PID */
        bool proc_completed; /**< process completion status */
        int proc_status; /**< process exit status */
} Process;

/**
 * @brief Initialize a @c Process object.
 * @param self object to initialize
 * @param argc argument count for @p argv
 * @param argv array of arguments for process command
 * @param pid process PID
 * @param completed whether or not the process has completed
 * @param status process exit status
 */
void process_ctor(Process *self, size_t argc, char **argv, pid_t pid,
                  bool completed, int status);

/**
 * @brief Reset @p self to default values and free any associated memory.
 * @param self object to free
 */
void process_dtor(Process *self);

/**
 * @brief Launches a new process.
 * @param proc process to launch
 * @param pgid process PGID
 * @param infile STDIN file stream
 * @param outfile STDOUT file stream
 * @param foreground whether or not the process is to run in the foreground
 */
void process_launch(Process *proc, pid_t pgid, char *infile, char *outfile,
                    bool foreground);

#endif //SMALLSH_PROCESS_H
