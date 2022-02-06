/**
 * @file process.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a single child process.
 *
 * Ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 */
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "job-control/process.h"
#include "core/shell-attrs.h"
#include "signals/installer.h"

/**
 * TLPI 34.2
 *
 * If the pid and pgid arguments specify the same process, then a new process
 * group is created, and the specified process is made the leader of the new group.
 */
void process_new_process_group(pid_t pgid)
{
        pid_t pid;

        pid = getpid();
        if (pgid == 0) {
                pgid = pid;
        }
        setpgid(pid, pgid);
}

void process_set_io_streams(char *infile, char *outfile)
{
        /* Get file descriptors for input/output files. */
        int fds[2];

        int stdin_flags = O_RDONLY;
        int stdout_flags = O_WRONLY | O_CREAT | O_TRUNC;
        int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

        fds[0] = open(infile, stdin_flags, mode);
        fds[1] = open(outfile, stdout_flags, mode);

        /* Set the standard input/output channels of the new process. */
        if (fds[0] != STDIN_FILENO) {
                dup2(fds[0], STDIN_FILENO);
                close(fds[0]);
        }
        if (fds[1] != STDOUT_FILENO) {
                dup2(fds[1], STDOUT_FILENO);
                close(fds[1]);
        }
}

void process_exec(char **argv)
{
        /* Exec new process. Make sure we exit. */
        execvp(argv[0], argv);
        perror("execvp");
        _exit(1);
}

void process_launch(Process *proc, pid_t pgid, char *infile, char *outfile,
                    bool foreground)
{
        if (shell_is_interactive) {
                /* Put the process into the process group and give the process
                 * group the terminal, if appropriate. */

                process_new_process_group(pgid);

                /**
                 * TLPI 34.5
                 *
                 * Within a session, only one process can be in the foreground
                 * at a particular moment. The foreground process group
                 * is the only process that can freely read and write on the
                 * controlling terminal. If the calling process has a controlling
                 * terminal, and the file descriptor fd refers to that terminal,
                 * then tcsetpgrp() sets the foreground process group of the
                 * terminal to the value specified in pgid, which must match
                 * the process group ID of one of the processes in the
                 * calling process's session.
                 */
                if (foreground) {
                        tcsetpgrp(shell_terminal, pgid);
                }

                /* Set the handling for job control signals back to the default */
                installer_uninstall_job_control_signals();
        }

        /* Set the standard input/output channels of the new process. */
        process_set_io_streams(infile, outfile);

        /* Exec new process. Make sure we exit. */
        process_exec(proc->argv);
}
