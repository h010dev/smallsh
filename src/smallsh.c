#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "smallsh.h"

#include "interpreter/parser.h"
#include "builtins/cd.h"
#include "signals/handler.h"
#include "job-control/job-table.h"
#include "signals/installer.h"
#include "core/shell-attrs.h"
#include "job-control/job-control.h"

#define SMALLSH_MAX_JOBS 200

static volatile int smallsh_errno = 0;
static volatile bool smallsh_fg_only_mode = false;
static volatile pid_t smallsh_jobs[SMALLSH_MAX_JOBS] = { 0 };
static volatile int smallsh_job_count = 0;
static volatile sig_atomic_t arr_len = 0;
static volatile sig_atomic_t pid_arr[SMALLSH_MAX_JOBS];
static volatile sig_atomic_t status_arr[SMALLSH_MAX_JOBS];

static JobTable job_table;

void smallsh_init(void)
{
        /* see if we are running interactively */
        shell_terminal = STDIN_FILENO;
        shell_is_interactive = isatty(shell_terminal);

        printf("orig tcpgrp=%d\n", tcgetpgrp(shell_terminal));
        printf("orig pgrp=%ld\n", (long) getpgrp());
        if (shell_is_interactive) {
                /* loop until we are in the foreground */
                while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp())) {
                        kill(-shell_pgid, SIGTTIN);
                }

                /* ignore interactive and job-control signals */
                installer_install_job_control_signals();

                /* put ourselves in our own process group */
                shell_pgid = getpid();
                printf("pid=%5ld, ppid=%5ld, pgrp=%5ld, sid=%5ld\n", (long) getpid(),
                       (long) getppid(), (long) getpgrp(), (long) getsid(0));
                errno = 0;
                int status = setpgid(shell_pgid, shell_pgid);
                if (status == -1) {
                        if (errno == EPERM && shell_pgid == getpgrp()) {
                                // process is session leader; do nothing
                                // this only happens when running from clion
                        } else {
                                perror("Couldn't put shell in its own process group");
                                _exit(1);
                        }
                }

                /* grab control of the terminal */
                tcsetpgrp(shell_terminal, shell_pgid);
                printf("final tcpgrp=%d\n", tcgetpgrp(shell_terminal));

                /* save default terminal attributes for shell */
                tcgetattr(shell_terminal, &shell_tmodes);
        }
}

int smallsh_run(void)
{
        // init shell
        smallsh_init();

        // register signals
        smallsh_register_signals();
        file_global_var = 2;

        job_table_ctor(&job_table);

        // run event loop
        while (1) {
                ssize_t n_read;
                char *cmd;
                int status;

                n_read = smallsh_read_command(&cmd);
                if (n_read == -1) {
                        return EXIT_FAILURE;
                }

                status = smallsh_eval2(cmd);
                if (status == -1) {
                        free(cmd);
                        return EXIT_FAILURE;
                } else if (status == 1) {
                        free(cmd);
                        return EXIT_SUCCESS;
                }

                free(cmd);
        }

        job_table_dtor(&job_table);
}

int smallsh_register_signals(void)
{
        /*
        struct sigaction new_action, old_action;
        new_action.sa_handler = signal_handler_handle;
        sigfillset(&new_action.sa_mask);
        new_action.sa_flags = SA_RESTART;
        sigaction(SIGINT, NULL, &old_action);
        if (old_action.sa_handler != SIG_IGN)
                sigaction(SIGINT, &new_action, NULL);
        sigaction(SIGTSTP, NULL, &old_action);
        if (old_action.sa_handler != SIG_IGN)
                sigaction(SIGTSTP, &new_action, NULL);
        */

        return 0;
}

ssize_t smallsh_read_command(char **cmd)
{
        ssize_t n_read;
        size_t len = 0;

        // shell prompt
        if(fputs(": ", stdout) == EOF) {
                // error
                return EXIT_FAILURE;
        }

        // read input
        errno = 0;
        n_read = getline(cmd, &len, stdin);
        if (n_read == -1) {
                return -1;
        }

        return n_read;
}

int smallsh_eval2(char *cmd)
{
        Parser parser;
        parser_ctor(&parser);

        ssize_t n_stmts = parser.parse(&parser, cmd);
        if (n_stmts == -1) {
                return -1;
        }
        Statement **stmts = parser.get_statement(&parser);

        for (ssize_t i = 0; i < n_stmts; i++) {
                if ((stmts[i]->stmt_flags & FLAGS_BUILTIN) == 0) {
                        // non-builtin
                        char **argv = stmts[i]->stmt_cmd->cmd_argv;
                        Process job_proc = {
                                .argv = argv,
                                .proc_pid = 0,
                                .proc_completed = false,
                                .proc_status = 0,
                        };
                        Statement *stmt = stmts[i];
                        StmtStdin *st_in = stmt->stmt_stdin;
                        char *infile = st_in[i].stdin_streams[st_in[i].stdin_num_streams];
                        StmtStdout *st_out = stmt->stmt_stdout;
                        char *outfile = st_out[i].stdout_streams[st_out[i].stdout_num_streams];
                        Job *job = malloc(sizeof(Job));
                        job->job_cmd = strdup(cmd);
                        job->job_proc = job_proc;
                        job->job_pgid = getpgrp();
                        if (infile != NULL) {
                                job->job_stdin = strdup(infile);
                        }
                        if (outfile != NULL) {
                                job->job_stdout = strdup(outfile);
                        }
                        job_table.add_job(&job_table, job);
                        bool foreground;
                        if ((stmts[i]->stmt_flags & FLAGS_BGCTRL) == 0) {
                                foreground = true;
                        } else{
                                foreground = false;
                        }
                        job_control_launch_job(&job, foreground);
                } else {
                        // builtin
                        char *cmd_name = stmts[i]->stmt_cmd->cmd_argv[0];
                        if (strcmp("exit", cmd_name) == 0) {
                                return 1;
                        } else if (strcmp("cd", cmd_name) == 0) {
                                char *dirname = stmts[i]->stmt_cmd->cmd_argv[1];
                                cd(dirname);
                        }
                }
        }

        parser_dtor(&parser);
        return 0;
}

int smallsh_eval(char *cmd)
{
        Parser parser;
        parser_ctor(&parser);

        ssize_t n_stmts = parser.parse(&parser, cmd);
        if (n_stmts == -1) {
                return -1;
        }
        Statement **stmts = parser.get_statement(&parser);

        for (ssize_t i = 0; i < n_stmts; i++) {
                if ((stmts[i]->stmt_flags & FLAGS_BUILTIN) == 0) {
                        // non-builtin
                        smallsh_errno = 0;
                        smallsh_exec(stmts[i]);
                } else {
                        // builtin
                        char *cmd_name = stmts[i]->stmt_cmd->cmd_argv[0];
                        if (strcmp("exit", cmd_name) == 0) {
                                return 1;
                        } else if (strcmp("cd", cmd_name) == 0) {
                                char *dirname = stmts[i]->stmt_cmd->cmd_argv[1];
                                cd(dirname);
                        }
                }
        }

        parser_dtor(&parser);
        return 0;
}

void smallsh_exec(Statement *stmt)
{
        pid_t pipeline_pgid = 69420;
        pid_t spawn_pid = fork();
        switch (spawn_pid) {
                case -1:
                {
                        perror("fork() failed\n");
                        smallsh_errno = -1;
                        break;
                }
                case 0:
                {
                        // set group id (34.2)
                        if (setpgid(0, pipeline_pgid) == -1) {
                                // error
                        }

                        // for bg
                        // copy stmt data
                        // signal parent to continue
                        // exec
                        smallsh_redirect(stmt->stmt_stdin->stdin_streams,
                                         stmt->stmt_stdout->stdout_streams);

                        int res = execvp(stmt->stmt_cmd->cmd_argv[0],
                                         stmt->stmt_cmd->cmd_argv);
                        if (res == -1) {
                                smallsh_errno = -1;
                                printf("command not found\n");
                        }
                        _exit(smallsh_errno);
                }
                default:
                {
                        // set group id (34.2)
                        if (setpgid(spawn_pid, pipeline_pgid) == -1 && errno != EACCES) {
                                // error
                        }

                        int exit_status;

                        if ((stmt->stmt_flags & FLAGS_BGCTRL) != 0) {
                                // background cmd
                                return;
                        }

                        pid_t child = waitpid(spawn_pid, &exit_status, 0);
                        (void) child;
                        if (WIFEXITED(exit_status)) {
                                smallsh_errno = WEXITSTATUS(exit_status);
                                //printf("process exited normally\n");
                        } else if (WIFSIGNALED(exit_status)) {
                                smallsh_errno = WTERMSIG(exit_status);
                                //printf("process was interrupted by signal\n");
                        } else {
                                //printf("undefined status");
                                smallsh_errno = -1;
                        }
                        break;
                }
        }
}

void smallsh_redirect(char **stdin_streams, char **stdout_streams)
{
        // tlpi 27.4
        // this only keeps last fd open for writing, which is
        // similar to bash behavior. see https://stackoverflow.com/a/60858754/11595201
        // all other files preceding final are truncated
        int stdin_flags = O_RDONLY;
        int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        while (*stdin_streams != NULL) {
                int fd = open(*stdin_streams, stdin_flags, mode);
                if (fd != STDIN_FILENO) {
                        dup2(fd, STDIN_FILENO);
                        close(fd);
                }
                stdin_streams++;
        }

        int stdout_flags = O_WRONLY | O_CREAT | O_TRUNC;
        while (*stdout_streams != NULL) {
                int fd = open(*stdout_streams, stdout_flags, mode);
                if (fd != STDOUT_FILENO) {
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                }
                stdout_streams++;
        }
}
