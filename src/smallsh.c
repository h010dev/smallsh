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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "smallsh.h"
#include "globals.h"
#include "builtins/builtins.h"
#include "error.h"
#include "events/events.h"
#include "job-control/job-control.h"
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
int smallsh_status = 0;

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
 * @brief Evaluate a command entered by the user.
 * @param cmd command to evaluate
 * @return 0 or 1 on success, -1 on failure
 */
static int smallsh_eval(char *cmd)
{
        int status_;
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

        /* Parse command into statements for evaluation. */
        parser_ctor(&parser);

        n_stmts = parser.parse(&parser, cmd);
        if (n_stmts == -1) {
                parser_dtor(&parser);
                return -1; /* error */
        } else if (n_stmts == 0) {
                parser_dtor(&parser);
                smallsh_line_buffer = true;
                return 0; /* no statements parsed */
        }

        /* Can have multiple statements, but we only want first one. */
        stmt = parser.get_statements(&parser)[0];

        /* Statement is not a builtin. */
        if ((stmt->stmt_flags & FLAGS_BUILTIN) == 0) {
                /* Create process object. */
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
                        smallsh_line_buffer = true;
                } else {
                        if (!shell_is_interactive) {
                                write(STDOUT_FILENO, "\n", 1);
                        }
                        outfile = NULL;
                }

                if ((stmt->stmt_flags & FLAGS_BGCTRL) == 0 || smallsh_fg_only_mode) {
                        foreground = true;
                } else{
                        foreground = false;
                }

                if (smallsh_fg_only_mode && (stmt->stmt_flags & FLAGS_BGCTRL) != 0 && outfile == NULL) {
                        smallsh_line_buffer = true;
                }

                /* Create job object. */
                job = malloc(sizeof(Job));
                job_ctor(job, job_proc, 0, infile, outfile, !foreground);

                /* Add job to job table. */
                job_table.add_job(&job_table, job);

                /* Run job. */
                status_ = job_control_launch_job(&job, foreground);
        }
                /* Statement is a builtin. */
        else {
                /* Determine builtin name and run it. */
                cmd_name = stmt->stmt_cmd->cmd_argv[0];
                if (strcmp("exit", cmd_name) == 0) {
                        status_ = 1;
                        smallsh_line_buffer = true;
                } else if (strcmp("cd", cmd_name) == 0) {
                        char *dirname = stmt->stmt_cmd->cmd_argv[1];
                        cd(dirname);
                        status_ = 0;
                        smallsh_line_buffer = true;
                } else if (strcmp("status", cmd_name) == 0) {
                        status(smallsh_status);
                        status_ = 0;
                } else {
                        /* error */
                        status_ = -1;
                }
        }

        parser_dtor(&parser);

        return status_;
}

/**
 * @brief Inspects flag set by SIGTSTP handler to determine if foreground-only
 * mode has been triggered.
 *
 * Sets global variable @c smallsh_fg_only_mode accordingly.
 *
 * Source: https://edstem.org/us/courses/16718/discussion/1067170
 * @return true if fg mode has changed, false otherwise
 */
static bool smallsh_fg_only_mode_triggered(void)
{
        bool trigger;

        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGTSTP);

        sigprocmask(SIG_BLOCK, &mask, NULL);

        trigger = false;
        if (smallsh_fg_only_mode != fg_flag) {
                smallsh_fg_only_mode = !smallsh_fg_only_mode;
                trigger = true;
        }

        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        return trigger;
}

/**
 * @brief Initialize shell and bring it to the foreground process group.
 *
 * Do not proceed until this is the case.
 */
static void smallsh_init(void)
{
        int status_;

        /*
         * See if we are running interactively.
         *
         * When running from a test script, STDIN will not be a tty, and thus
         * we will not be in interactive mode.
         */
        shell_terminal = STDIN_FILENO;
        shell_is_interactive = isatty(shell_terminal);

        if (shell_is_interactive) {
                shell_pgid = getpgrp();

                /* Loop until we are in the foreground. */
                while (tcgetpgrp(shell_terminal) != shell_pgid) {
                        kill(-shell_pgid, SIGTTIN);
                }
        }

        /* Ignore interactive and job-control signals. */
        installer_install_job_control_signals();

        /* Put ourselves in our own process group. */
        shell_pgid = getpid();

        errno = 0;
        status_ = setpgid(shell_pgid, shell_pgid);
        if (status_ == -1) {
                if (errno == EPERM && shell_pgid == getpgrp()) {
                        /*
                         * Process is session leader; do nothing.
                         *
                         * This is only triggered via CLion's run mode.
                         */
                        perror("shell is session leader");
                } else {
                        perror("Couldn't put shell in its own process group");
                        _exit(1);
                }
        }

        if (shell_is_interactive) {
                /* Grab control of the terminal. */
                tcsetpgrp(shell_terminal, shell_pgid);
        }
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

        /* Prompt user for command. */
        if (write(STDOUT_FILENO, ": ", 2) == -1) {
                perror("write");
                _exit(1);
        }

        /* Read input command from user. */
        len = 0;
        errno = 0;
        n_read = getline(cmd, &len, stdin);
        if (n_read == -1 && errno != 0) {
                fprintf(stderr, "getline(): %s\n", strerror(errno));
                fflush(stderr);
                return -1;
        }

        return n_read;
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
        int status_;

        /* Setup event listener to catch signal events and related data. */
        status_ = events_init();
        if (status_ == -1) {
                fprintf(stderr, "events_init()");
                fflush(stderr);
                return EXIT_FAILURE;
        }

        smallsh_init();

        job_table_ctor(&job_table);

        /* Run event loop forever until shell termination. */
        do {
                /* Notify user about new job-control events. */
                status_ = events_notify();
                if (status_ == -1) {
                        fprintf(stderr, "events_notify()\n");
                        fflush(stderr);
                        status_ = EXIT_FAILURE;
                        break;
                }

                /* Read command from user. */
                cmd = NULL;
                smallsh_errno = 0;
                n_read = smallsh_read_input(&cmd);
                if (n_read == -1) {
                        /*
                        fprintf(stderr, "smallsh_read_input()\n");
                        fflush(stderr);
                        status_ = EXIT_FAILURE;
                        break;
                        */
                        free(cmd);
                        continue;
                }

                /*
                 * Check if SIGTSTP triggered fg-only mode on/off and update
                 * shell's state accordingly.
                 */
                if (smallsh_fg_only_mode_triggered()) {
                        /* Re-prompt user. */
                }

                /* Evaluate command. */
                status_ = smallsh_eval(cmd);
                if (status_ == -1) {
                        if (smallsh_errno == SMSH_EOPEN) {
                                /* Do nothing. */
                                smallsh_errno = 0;
                                smallsh_status = 1;
                        } else {
                                free(cmd);
                                status_ = EXIT_FAILURE;
                                break;
                        }
                } else if (status_ == 1) {
                        free(cmd);
                        status_ = EXIT_SUCCESS;
                        break;
                }

                if (!shell_is_interactive && smallsh_line_buffer) {
                        write(STDOUT_FILENO, "\n", 1);
                }
                smallsh_line_buffer = false;

                free(cmd);
        } while (1);

        if (status_ == EXIT_SUCCESS) {
                exit_();
        }
        if (!shell_is_interactive && smallsh_line_buffer) {
                write(STDOUT_FILENO, "\n", 1);
        }

        job_table_dtor(&job_table);

        events_cleanup();

        return status_;
}
