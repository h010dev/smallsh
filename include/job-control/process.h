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
        char **args; /**< process arguments */
        pid_t pid; /**< process PID */
        bool has_completed; /**< process completion SH_status */
        int status; /**< process exit SH_status */
} SH_Process;

/**
 * @brief Initialize new Process object.
 * @param n_args argument count for @p argv
 * @param args array of arguments for process command
 * @return new Process object
 */
SH_Process *SH_CreateProcess(size_t n_args, char **args);

/**
 * @brief Reset @p self to default values and free any associated memory.
 * @param proc object to free
 */
void SH_DestroyProcess(SH_Process *proc);

/**
 * @brief Launches a new process.
 * @param proc process to launch
 * @param pgid process PGID
 * @param infile STDIN file stream
 * @param outfile STDOUT file stream
 * @param foreground whether or not the process is to run in the foreground
 */
void SH_LaunchProcess(SH_Process *proc, pid_t pgid, char *infile, char *outfile,
                      bool foreground);

#endif //SMALLSH_PROCESS_H
