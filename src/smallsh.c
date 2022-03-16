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

#include "builtins/builtins.h"
#include "error.h"
#include "events/events.h"
#include "globals.h"
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
        SH_Process *proc;
        StmtStdin *st_in;
        StmtStdout *st_out;
        char *infile, *outfile, *cmd_name;
        size_t n_in, n_out;
        bool foreground;
        SH_Job *job;

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
                proc = SH_CreateProcess(stmt->stmt_cmd->cmd_argc, stmt->stmt_cmd->cmd_argv);

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
                        if (!smallsh_interactive_mode) {
#ifdef TEST_SCRIPT
                                /* Add newline to end of empty non-echo commands. */
                                if (strcmp("echo", stmt->stmt_cmd->cmd_argv[0]) != 0) {
                                        write(STDOUT_FILENO, "\n", 1);
                                }
#endif
                        }
                        outfile = NULL;
                }

                if ((stmt->stmt_flags & FLAGS_BGCTRL) == 0 || smallsh_fg_only_mode) {
                        foreground = true;
                } else{
                        foreground = false;
                }

                if (!smallsh_interactive_mode) {
#ifdef TEST_SCRIPT
                        /* Some fun magic to make output pretty for test script. */
                        if (smallsh_fg_only_mode) {
                                if ((stmt->stmt_flags & FLAGS_BGCTRL) != 0) {
                                        if (outfile == NULL) {
                                                smallsh_line_buffer = true;
                                        }
                                }
                        }
#endif
                }
                /* Create job object. */
                job = SH_CreateJob(cmd, proc, infile, outfile, !foreground);

                /* Add job to job table. */
                SH_JobTableAddJob(job_table, job);

                /* Run job. */
                status_ = SH_JobControlLaunchJob(&job, foreground);
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
                        SH_cd(dirname);
                        status_ = 0;
                        smallsh_line_buffer = true;
                } else if (strcmp("status", cmd_name) == 0) {
                        SH_status();
                        status_ = 0;
                } else {
                        /* Error */
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
 */
static void smallsh_inspect_fg_only_mode_flag(void)
{
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGTSTP);

        sigprocmask(SIG_BLOCK, &mask, NULL);

        if (smallsh_fg_only_mode != smallsh_fg_only_mode_flag) {
                smallsh_fg_only_mode = !smallsh_fg_only_mode;
        }

        sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

/**
 * @brief Initialize shell and bring it to the foreground process group.
 *
 * Do not proceed until this is the case (but make an exception for the
 * test script in this case).
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
        smallsh_shell_terminal = STDIN_FILENO;
        smallsh_interactive_mode = isatty(smallsh_shell_terminal);

        if (smallsh_interactive_mode) {
                smallsh_shell_pgid = getpgrp();

                /* Loop until we are in the foreground. */
                while (tcgetpgrp(smallsh_shell_terminal) != smallsh_shell_pgid) {
                        kill(-smallsh_shell_pgid, SIGTTIN);
                }
        }

        /* Ignore interactive and job-control signals. */
        installer_install_job_control_signals();

        /* Put ourselves in our own process group. */
        smallsh_shell_pgid = getpid();

        errno = 0;
        status_ = setpgid(smallsh_shell_pgid, smallsh_shell_pgid);
        if (status_ == -1) {
                if (errno == EPERM && smallsh_shell_pgid == getpgrp()) {
                        /*
                         * Process is session leader; do nothing.
                         *
                         * This is only triggered via CLion's run mode.
                         */
                } else {
                        print_error_msg("Couldn't put shell in its own process group");
                        _exit(1);
                }
        }

        if (smallsh_interactive_mode) {
                /* Grab control of the terminal. */
                tcsetpgrp(smallsh_shell_terminal, smallsh_shell_pgid);
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
                print_error_msg("write");
                _exit(1);
        }

        /* Read input command from user. */
        len = 0;
        errno = 0;
        n_read = getline(cmd, &len, stdin);
        if (n_read == -1 && errno != 0) {
                print_error_msg("getline()");
                return -1;
        }
#ifdef TEST_SCRIPT_ECHO_COMMANDS
        /* Displays commands as if they were typed live in terminal */
        fprintf(stdout, "%s", *cmd);
        fflush(stdout);
#endif
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
 * OBJECTS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
volatile int smallsh_errno = 0;
bool smallsh_line_buffer = false;
int smallsh_interactive_mode = 0;
int smallsh_fg_only_mode = 0;
SH_JobTable *job_table = NULL;
int smallsh_shell_terminal = 0;
int smallsh_shell_pgid = 0;
SH_Channel *sigchld_channel = NULL;
SH_Receiver *receiver = NULL;
SH_Sender *sender = NULL;
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
 * @brief Run shell's main event loop until terminated.
 */
int main(void)
{
        ssize_t n_read;
        char *cmd;
        int status_;

        /* Setup event listener to catch signal events and related data. */
        status_ = SH_InitEvents();
        if (status_ == -1) {
                print_error_msg("SH_InitEvents()");
                _exit(1);
        }

        smallsh_init();

        job_table = SH_CreateJobTable();

        /* Run event loop forever until shell termination. */
        do {
                /* Notify user about new job-control events. */
                status_ = SH_NotifyEvents();
                if (status_ == -1) {
                        print_error_msg("SH_NotifyEvents()");
                        status_ = EXIT_FAILURE;
                        break;
                }

                /* Read command from user. */
                cmd = NULL;
                n_read = smallsh_read_input(&cmd);
                if (n_read == -1) {
                        print_error_msg("smallsh_read_input()");
                        status_ = EXIT_FAILURE;
                        break;
                }

                /*
                 * Check if SIGTSTP triggered fg-only mode on/off and update
                 * shell's state accordingly.
                 */
                smallsh_inspect_fg_only_mode_flag();

                /* Evaluate command. */
                status_ = smallsh_eval(cmd);
                if (status_ == -1) {
                        free(cmd);
                        status_ = EXIT_FAILURE;
                        break;
                } else if (status_ == 1) {
                        free(cmd);
                        status_ = EXIT_SUCCESS;
                        break;
                }

                if (!smallsh_interactive_mode) {
#ifdef TEST_SCRIPT
                        /* Add an extra newline to make output pretty. */
                        if (smallsh_line_buffer) {
                                write(STDOUT_FILENO, "\n", 1);
                        }
                        smallsh_line_buffer = false;
#endif
                }

                free(cmd);
        } while (1);

        SH_exit(status_);
}
