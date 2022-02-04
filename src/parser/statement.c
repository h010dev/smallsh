/**
 * @file statement.c
 * @author Mohamed Al-Hussein
 * @date 02 Feb 2022
 * @brief For creating command statements to later be executed.
 */
#include <stdlib.h>
#include <stdio.h>

#include "parser/statement.h"

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
Statement *statement_new(void)
{
        Statement *stmt = malloc(sizeof(Statement));

        // statement command init
        stmt->stmt_cmd = malloc(sizeof(StmtCmd));
        stmt->stmt_cmd->cmd_argc = 0;
        stmt->stmt_cmd->cmd_argv = malloc(sizeof(char *));
        stmt->stmt_cmd->cmd_argv[0] = NULL;

        // statement io redirection: stdin init
        stmt->stmt_stdin = malloc(sizeof(StmtStdin));
        stmt->stmt_stdin->stdin_num_streams = 0;
        stmt->stmt_stdin->stdin_streams = malloc(sizeof(char *));
        stmt->stmt_stdin->stdin_streams[0] = NULL;

        // statement io redirection: stdout init
        stmt->stmt_stdout = malloc(sizeof(StmtStdout));
        stmt->stmt_stdout->stdout_num_streams = 0;
        stmt->stmt_stdout->stdout_streams = malloc(sizeof(char *));
        stmt->stmt_stdout->stdout_streams[0] = NULL;

        // statement flags init
        stmt->stmt_flags = 0;

        return stmt;
}

void statement_del(Statement **stmt)
{
        // statement command delete
        for (size_t i = 0; i < (*stmt)->stmt_cmd->cmd_argc; i++) {
                free((*stmt)->stmt_cmd->cmd_argv[i]);
                (*stmt)->stmt_cmd->cmd_argv[i] = NULL;
        }
        free((*stmt)->stmt_cmd->cmd_argv);
        (*stmt)->stmt_cmd->cmd_argc = 0;
        (*stmt)->stmt_cmd->cmd_argv = NULL;
        free((*stmt)->stmt_cmd);
        (*stmt)->stmt_cmd = NULL;

        // statement io redirection: stdin delete
        for (size_t i = 0; i < (*stmt)->stmt_stdin->stdin_num_streams; i++) {
                free((*stmt)->stmt_stdin->stdin_streams[i]);
                (*stmt)->stmt_stdin->stdin_streams[i] = NULL;
        }
        free((*stmt)->stmt_stdin->stdin_streams);
        (*stmt)->stmt_stdin->stdin_num_streams = 0;
        (*stmt)->stmt_stdin->stdin_streams = NULL;
        free((*stmt)->stmt_stdin);
        (*stmt)->stmt_stdin = NULL;

        // statement io redirection: stdout delete
        for (size_t i = 0; i < (*stmt)->stmt_stdout->stdout_num_streams; i++) {
                free((*stmt)->stmt_stdout->stdout_streams[i]);
                (*stmt)->stmt_stdout->stdout_streams[i] = NULL;
        }
        free((*stmt)->stmt_stdout->stdout_streams);
        (*stmt)->stmt_stdout->stdout_num_streams = 0;
        (*stmt)->stmt_stdout->stdout_streams = NULL;
        free((*stmt)->stmt_stdout);
        (*stmt)->stmt_stdout = NULL;

        // statement flags delete
        (*stmt)->stmt_flags = FLAGS_NONE;

        // statement delete
        free(*stmt);
        *stmt = NULL;
}

/* *****************************************************************************
 * CLASS METHODS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void statement_print(const Statement *self)
{
        printf("STATEMENT(\n");
        printf("\tFLAGS(\n");
        if (self->stmt_flags == FLAGS_BGCTRL) {
                printf("\t\tBG_CTRL,\n");
        }
        printf("\t)\n");

        printf("\tCMD(\n");
        for (size_t i = 0; i < self->stmt_cmd->cmd_argc; i++) {
                printf("\t\tARG [%zu] = %s,\n", i, self->stmt_cmd->cmd_argv[i]);
        }
        printf("\t)\n");

        printf("\tSTDIN(\n");
        for (size_t i = 0; i < self->stmt_stdin->stdin_num_streams; i++) {
                printf("\t\t[%zu] = %s,\n", i, self->stmt_stdin->stdin_streams[i]);
        }
        printf("\t)\n");

        printf("\tSTDOUT(\n");
        for (size_t i = 0; i < self->stmt_stdout->stdout_num_streams; i++) {
                printf("\t\t[%zu] = %s,\n", i, self->stmt_stdout->stdout_streams[i]);
        }
        printf("\t)\n");
        printf(")\n");
}
