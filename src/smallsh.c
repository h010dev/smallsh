/**
 * @file smallsh.c
 * @author Mohamed Al-Hussein
 * @date 01 Feb 2022
 * @brief Contains core shell functions.
 *
 * Some of the ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "smallsh.h"
#include "builtins/cd.h"
#include "core/dtos.h"
#include "core/shell-attrs.h"
#include "job-control/job-control.h"
#include "job-control/job-table.h"
#include "interpreter/parser.h"
#include "signals/installer.h"

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
 * OBJECTS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
static volatile int smallsh_errno = 0;
static JobTable job_table;
static bool smallsh_fg_only_mode = false;

static fd_set readfds;
static int ready, nfds;
static struct timeval timeout;

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
 * @brief Initialize shell and bring it to the foreground process group.
 *
 * Do not proceed until this is the case.
 */
static void smallsh_init(void)
{
        /*
         * See if we are running interactively.
         */
        shell_terminal = STDIN_FILENO;
        shell_is_interactive = isatty(shell_terminal);

#ifdef DEBUG
        printf("orig tcpgrp=%d\n", tcgetpgrp(shell_terminal));
        printf("orig pgrp=%ld\n", (long) getpgrp());
#endif

        if (shell_is_interactive) {
                /*
                 * Loop until we are in the foreground.
                 */
                shell_pgid = getpgrp();
                while (tcgetpgrp(shell_terminal) != shell_pgid) {
                        kill(-shell_pgid, SIGTTIN);
                }

                /*
                 * Ignore interactive and job-control signals.
                 */
                installer_install_job_control_signals();

                /* Put ourselves in our own process group.
                 */
                shell_pgid = getpid();

#ifdef DEBUG
                printf("pid=%5ld, ppid=%5ld, pgrp=%5ld, sid=%5ld\n", (long) getpid(),
                       (long) getppid(), (long) getpgrp(), (long) getsid(0));
#endif

                errno = 0;
                int status = setpgid(shell_pgid, shell_pgid);
                if (status == -1) {
                        if (errno == EPERM && shell_pgid == getpgrp()) {
                                /*
                                 * Process is session leader; do nothing.
                                 *
                                 * This is only triggered via CLion's run mode.
                                 */
                                perror("shell is session leader\n");
                        } else {
                                perror("Couldn't put shell in its own process group");
                                _exit(1);
                        }
                }

                /*
                 * Grab control of the terminal.
                 */
                tcsetpgrp(shell_terminal, shell_pgid);

#ifdef DEBUG
                printf("final tcpgrp=%d\n", tcgetpgrp(shell_terminal));
#endif
        }
}

/**
 * @brief Configure event listeners to capture new signal-generated events.
 *
 * Source: TLPI section 63.9
 */
static void smallsh_config_event_listeners(void)
{
        int flags;

        /*
         * Open new pipes for signal events.
         */
        if (pipe(pipe_sigchld) == -1) {
                fprintf(stderr, "pipe\n");
                _exit(1);
        }
        if (pipe(pipe_sigtstp) == -1) {
                fprintf(stderr, "pipe\n");
                _exit(1);
        }

        /*
         * Initialize pipes for select poll.
         */
        FD_ZERO(&readfds);
        FD_SET(pipe_sigchld[0], &readfds);
        nfds = pipe_sigchld[0] + 1;
        FD_SET(pipe_sigchld[1], &readfds);
        if (pipe_sigtstp[1] > nfds) {
                nfds = pipe_sigtstp[1] + 1;
        }

        /*
         * Configure pipes to be non-blocking.
         */
        flags = fcntl(pipe_sigchld[0], F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "fcntl-F_GETFL\n");
                _exit(1);
        }
        flags |= O_NONBLOCK;
        if (fcntl(pipe_sigchld[0], F_SETFL, flags) == -1) {
                fprintf(stderr, "fcntl-F_SETFL\n");
                _exit(1);
        }

        flags = fcntl(pipe_sigchld[1], F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "fcntl-F_GETFL\n");
                _exit(1);
        }
        flags |= O_NONBLOCK;
        if (fcntl(pipe_sigchld[1], F_SETFL, flags) == -1) {
                fprintf(stderr, "fcntl-F_SETFL\n");
                _exit(1);
        }

        flags = fcntl(pipe_sigtstp[0], F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "fcntl-F_GETFL\n");
                _exit(1);
        }
        flags |= O_NONBLOCK;
        if (fcntl(pipe_sigtstp[0], F_SETFL, flags) == -1) {
                fprintf(stderr, "fcntl-F_SETFL\n");
                _exit(1);
        }

        flags = fcntl(pipe_sigtstp[1], F_GETFL);
        if (flags == -1) {
                fprintf(stderr, "fcntl-F_GETFL\n");
                _exit(1);
        }
        flags |= O_NONBLOCK;
        if (fcntl(pipe_sigtstp[1], F_SETFL, flags) == -1) {
                fprintf(stderr, "fcntl-F_SETFL\n");
                _exit(1);
        }

        /*
         * Don't block; just poll for file descriptors.
         *
         * Source: TLPI section 63.2.1
         */
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
}

/**
 * @brief Poll for new signal-related events and consume in.
 */
static void smallsh_consume_events(void)
{
        char ch;
        SigchldDTO dto;

        /*
         * Read in new data from signal-pipes.
         */
        errno = 0;
        while ((ready = select(nfds, &readfds, NULL, NULL, &timeout)) == -1 &&
               errno == EINTR)
                ;
        if (ready == -1 && errno != EINTR) {
                fprintf(stderr, "select\n");
                _exit(1);
        }

        /*
         * SIGTSTP signal(s) received.
         */
        if (FD_ISSET(pipe_sigtstp[0], &readfds)) {
                ch = 'a'; /* default garbage value */

#ifdef DEBUG
                printf("A tstp signal was caught\n");
#endif

                for (;;) {
                        /*
                         * Drain pipe of any data.
                         */
                        if (read(pipe_sigtstp[0], &ch, 1) == -1) {
                                if (errno == EAGAIN) {
                                        break;
                                } else {
                                        fprintf(stderr, "read\n");
                                        _exit(1);
                                }
                        }

#ifdef DEBUG
                        putc(ch, stdout);
#endif

                }
                /*
                 * Switch foreground-only mode.
                 */
                if (ch == 'x') {
                        smallsh_fg_only_mode = !smallsh_fg_only_mode;
                        if (smallsh_fg_only_mode) {
                                printf("Entering foreground-only mode (& is now ignored)\n");
                        } else {
                                printf("Exiting foreground-only mode\n");
                        }
                }
        }
        /*
         * SIGCHLD signal(s) received.
         */
        else if (FD_ISSET(pipe_sigchld[0], &readfds)) {

#ifdef DEBUG
                printf("A signal was caught\n");
#endif

                for (;;) {
                        /*
                         * Drain pipe and for any SIGCHLD DTOs.
                         */
                        if (read(pipe_sigchld[0], &dto, sizeof(dto)) == -1) {
                                if (errno == EAGAIN) {
                                        break;
                                } else {
                                        fprintf(stderr, "read\n");
                                        _exit(1);
                                }
                        }

#ifdef DEBUG
                        printf("Received SigchldDTO: pid=%d, status=%d\n",
                               dto.chld_pid, dto.chld_status);
#endif

                        /*
                         * Update relevant job in job table.
                         */
                        job_table.update(&job_table, dto.chld_pid, dto.chld_status);
                }
        }

        /*
         * Reset pipe file descriptors.
         */
        FD_ZERO(&readfds);
        FD_SET(pipe_sigchld[0], &readfds);
        FD_SET(pipe_sigtstp[0], &readfds);
}

/**
 * @brief Notify user of any new signal-related events.
 */
static void smallsh_notify_events(void)
{
        job_table.clean(&job_table);
}

/**
 * @brief Prompt user for command and read input to @p cmd.
 * @param cmd command to store input in
 * @return number of characters read
 */
static ssize_t smallsh_read_input(char **cmd)
{
        ssize_t n_read;
        size_t len;

        /*
         * Prompt user for command.
         */
        if(fputs(": ", stdout) == EOF) {
                return EXIT_FAILURE;
        }

        /*
         * Read input command from user.
         */
        len = 0;

        errno = 0;
        n_read = getline(cmd, &len, stdin);
        if (n_read == -1) {
                return -1;
        }

        return n_read;
}

/**
 * @brief Evaluate a command entered by the user.
 * @param cmd command to evaluate
 * @return 0 or 1 on success, -1 on failure
 */
static int smallsh_eval(char *cmd)
{
        int status;
        Parser parser;
        ssize_t n_stmts;
        Statement *stmt;
        Process *job_proc;
        StmtStdin *st_in;
        StmtStdout *st_out;
        char *infile, *outfile, *cmd_name;
        size_t n_in, n_out;
        bool foreground;
        Job *job;

        /*
         * Parse command into statements for evaluation.
         */
        parser_ctor(&parser);

        n_stmts = parser.parse(&parser, cmd);
        if (n_stmts == -1) {
                parser_dtor(&parser);
                return -1; /* error */
        } else if (n_stmts == 0) {
                parser_dtor(&parser);
                return 0; /* no statements parsed */
        }

        /*
         * Can have multiple statements, but we only want first one.
         */
        stmt = parser.get_statement(&parser)[0];

        /*
         * Statement is not a builtin.
         */
        if ((stmt->stmt_flags & FLAGS_BUILTIN) == 0) {
                /*
                 * Create process object.
                 */
                job_proc = malloc(sizeof(Process));
                process_ctor(job_proc, stmt->stmt_cmd->cmd_argc,
                             stmt->stmt_cmd->cmd_argv, 0, false, 0);

                st_in = stmt->stmt_stdin;
                st_out = stmt->stmt_stdout;
                n_in = st_in->stdin_num_streams;
                n_out = st_out->stdout_num_streams;
                if (n_in > 0) {
                        infile = st_in->stdin_streams[n_in - 1];
                } else {
                        infile = NULL;
                }
                if (n_out > 0) {
                        outfile = st_out->stdout_streams[n_out - 1];
                } else {
                        outfile = NULL;
                }

                if ((stmt->stmt_flags & FLAGS_BGCTRL) == 0 || smallsh_fg_only_mode) {
                        foreground = true;
                } else{
                        foreground = false;
                }

                /*
                 * Create job object.
                 */
                job = malloc(sizeof(Job));
                job_ctor(job, job_proc, 0, infile, outfile, !foreground);

                /*
                 * Add job to job table.
                 */
                job_table.add_job(&job_table, job);

                /*
                 * Run job.
                 */
                job_control_launch_job(&job, foreground);
                status = 0;
        }
        /*
         * Statement is a builtin.
         */
        else {
                /*
                 * Determine builtin name and run it.
                 */
                cmd_name = stmt->stmt_cmd->cmd_argv[0];
                if (strcmp("exit", cmd_name) == 0) {
                        /* do cleanup here */
                        status = 1;
                } else if (strcmp("cd", cmd_name) == 0) {
                        char *dirname = stmt->stmt_cmd->cmd_argv[1];
                        cd(dirname);
                        status = 0;
                } else if (strcmp("status", cmd_name) == 0) {
                        /* return status here */
                        status = 0;
                } else {
                        /* error */
                        status = -1;
                }
        }

        parser_dtor(&parser);

        return status;
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
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
int smallsh_run(void)
{
        ssize_t n_read;
        char *cmd;
        int status;

        smallsh_init();

        /*
         * Setup event listeners to catch signal events and related data.
         */
        smallsh_config_event_listeners();

        job_table_ctor(&job_table);

        /*
         * Run event loop forever until shell termination.
         */
        do {
                /*
                 * Consume new events and notify user about them.
                 */
                smallsh_consume_events();
                smallsh_notify_events();

                /*
                 * Read command from user.
                 */
                cmd = NULL;
                n_read = smallsh_read_input(&cmd);
                if (n_read == -1) {
                        status = EXIT_FAILURE;
                        break;
                }

                /*
                 * Evaluate command.
                 */
                status = smallsh_eval(cmd);
                if (status == -1) {
                        free(cmd);
                        status = EXIT_FAILURE;
                        break;
                } else if (status == 1) {
                        free(cmd);
                        status = EXIT_SUCCESS;
                        break;
                }

                free(cmd);
        } while (1);

        job_table_dtor(&job_table);

        return status;
}
