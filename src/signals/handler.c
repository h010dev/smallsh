/**
 * @date 04 Feb 2022
 * @param sig
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "signals/handler.h"

// 34.6.1
void dispatcher_dispatch_sighup(pid_t pid);
void handler_handle_sighup(int sig);

void handler_handle_sigint(int sig);
void handler_handle_sigtstp(int sig);

// tlpi 26.3
void handler_handle_sigchld(int sig)
{
        (void) sig;
        file_global_var = 1;

        int status, saved_errno;
        pid_t child_pid;

        saved_errno = errno;

        while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
                // check wait status value (26.1.3)
                // raise signal to usr type sig in smallsh ?
                continue;
        }

        if (child_pid == -1 && errno != ECHILD) {
                // error
        }

        errno = saved_errno;
}
