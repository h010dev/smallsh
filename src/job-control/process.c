/**
 * @file process.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a single child process.
 *
 * Some of the ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "job-control/process.h"
#include "signals/installer.h"
#include "globals.h"
#include "error.h"

/* *****************************************************************************
 * PRIVATE DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/**
 * @brief Execute program with @p argv as its arguments.
 *
 * Wrapper around exec* call that executes new program with @p argv
 * as its arguments, and displays error on failure.
 * @param argv arguments to pass to exec* function
 */
static void process_exec(char **argv)
{
        int status;

        errno = 0;
        status = execvp(argv[0], argv);
        if (status == -1) {
                fprintf(stderr, "-smallsh: %s: %s\n", argv[0], strerror(errno));
                fflush(stderr);
                _exit(1);
        }
}

/**
 * @brief Creates new process group and assigns current process as the leader.
 * @param pgid new process group
 */
static void process_set_group(pid_t *pgid)
{
        pid_t pid;

        pid = getpid();

        if (*pgid == 0) {
                *pgid = pid;
        }
        setpgid(pid, *pgid);
}

/**
 * @brief Opens IO streams for process.
 *
 * STDIN, STDOUT redirected to new streams.
 * @param infile filename to redirect STDIN to
 * @param outfile filename to redirect STDOUT to
 * @param foreground whether or not the process will run in foreground
 */
static int process_set_io_streams(char *infile, char *outfile, bool foreground)
{
        int status, stdin_flags, stdout_flags, mode;
        char *default_io;
        int fds[2];

        default_io = "/dev/null";

        stdin_flags = O_RDONLY;

        /* Create file for stdout stream if it doesn't exit. */
        stdout_flags = O_WRONLY | O_CREAT | O_TRUNC;

        /* -rw-rw---- */
        mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

        /*
         * Redirect to default stream if process will run in background, and
         * the user failed to specify any input/output redirections.
         */
        if (infile == NULL && !foreground) {
                infile = default_io;
        }

        if (outfile == NULL && !foreground) {
                outfile = default_io;
        }

        /*
         * At this point, infile/outfile can only be null if the process will
         * run in foreground and the user did not specify any redirections for
         * the respective streams.
         */

        /* Set the standard input stream of the new process. */
        if (infile != NULL) {
                /* Attempt to open input stream. */
                errno = 0;
                fds[0] = open(infile, stdin_flags, mode);
                if (fds[0] == -1) {
                        smallsh_errno = 1;
                        fprintf(stderr, "-smallsh: %s: %s\n", infile, strerror(errno));
                        fflush(stderr);
                        return -1;
                }

                /* Duplicate STDIN if stream is not already pointing to STDIN. */
                if (fds[0] != STDIN_FILENO) {
                        errno = 0;
                        status = dup2(fds[0], STDIN_FILENO);
                        if (status == -1) {
                                perror("dup2");
                                _exit(1);
                        }

                        errno = 0;
                        status = close(fds[0]);
                        if (status == -1) {
                                perror("close");
                                _exit(1);
                        }
                }
        }

        /* Set the standard output stream of the new process. */
        if (outfile != NULL) {
                /* Attempt to open input stream. */
                errno = 0;
                fds[1] = open(outfile, stdout_flags, mode);
                if (fds[1] == -1) {
                        smallsh_errno = 1;
                        fprintf(stderr, "-smallsh: %s: %s\n", outfile, strerror(errno));
                        fflush(stderr);
                        return -1;
                }

                /* Duplicate STDOUT if stream is not already pointing to STDOUT. */
                if (fds[1] != STDOUT_FILENO) {
                        errno = 0;
                        status = dup2(fds[1], STDOUT_FILENO);
                        if (status == -1) {
                                perror("dup2");
                                _exit(1);
                        }

                        errno = 0;
                        status = close(fds[1]);
                        if (status == -1) {
                                perror("close");
                                _exit(1);
                        }
                }
        }

        return 0;
}

/* *****************************************************************************
 * PUBLIC DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * CONSTRUCTORS + DESTRUCTORS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void process_ctor(Process *self, size_t argc, char **argv, pid_t pid,
                  bool completed, int status)
{
        /* Copy over argv array. */
        char **tmp = malloc((argc + 1) * sizeof(char *));
        if (tmp == NULL) {
                fprintf(stderr, "malloc\n");
                _exit(1);
        }
        self->argv = tmp;

        for (size_t i = 0; i < argc; i++) {
                errno = 0;
                self->argv[i] = strdup(argv[i]);
                if (self->argv[i] == NULL) {
                        perror("strdup");
                        _exit(1);
                }
        }

        /* Null-terminate list. */
        self->argv[argc] = NULL;

        /* Initialize remaining variables. */
        self->proc_pid = pid;
        self->proc_completed = completed;
        self->proc_status = status;
}

void process_dtor(Process *self)
{
        /* Free argv array. */
        for (size_t i = 0; self->argv[i] != NULL; i++) {
                free(self->argv[i]);
                self->argv[i] = NULL;
        }
        free(self->argv);
        self->argv = NULL;

        /* Reset remaining variables. */
        self->proc_pid = 0;
        self->proc_completed = false;
        self->proc_status = 0;
}

/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void process_launch(Process *proc, pid_t pgid, char *infile, char *outfile,
                    bool foreground)
{
        int status;

        process_set_group(&pgid);

        if (smallsh_interactive_mode) {
                /*
                 * If we are running in the foreground, set the process as the
                 * controlling foreground process.
                 */
                if (foreground) {
                        errno = 0;
                        status = tcsetpgrp(smallsh_shell_terminal, pgid);
                        if (status == -1) {
                                perror("tcsetpgrp");
                                _exit(1);
                        }
                }
        }

        installer_install_child_process_signals(foreground);

        smallsh_errno = 0;
        status = process_set_io_streams(infile, outfile, foreground);
        if (status == -1) {
                return;
        }

        process_exec(proc->argv);
}
