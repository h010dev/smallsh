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
Statement *Statement_new(void)
{
        Statement *stmt = malloc(sizeof(Statement));

        // statement command init
        stmt->cmd = malloc(sizeof(StmtCmd));
        stmt->cmd->argc = 0;
        stmt->cmd->argv = malloc(sizeof(char *));

        // statement io redirection: stdin init
        stmt->stdin_ = malloc(sizeof(StmtStdin));
        stmt->stdin_->n = 0;
        stmt->stdin_->streams = malloc(sizeof(char *));

        // statement io redirection: stdout init
        stmt->stdout_ = malloc(sizeof(StmtStdout));
        stmt->stdout_->n = 0;
        stmt->stdout_->streams = malloc(sizeof(char *));

        // statement flags init
        stmt->flags = 0;

        return stmt;
}

void Statement_del(Statement **stmt)
{
        // statement command delete
        for (size_t i = 0; i < (*stmt)->cmd->argc; i++) {
                free((*stmt)->cmd->argv[i]);
                (*stmt)->cmd->argv[i] = NULL;
        }
        free((*stmt)->cmd->argv);
        (*stmt)->cmd->argc = 0;
        (*stmt)->cmd->argv = NULL;
        //free((*stmt)->cmd);
        (*stmt)->cmd = NULL;

        // statement io redirection: stdin delete
        for (size_t i = 0; i < (*stmt)->stdin_->n; i++) {
                free((*stmt)->stdin_->streams[i]);
                (*stmt)->stdin_->streams[i] = NULL;
        }
        free((*stmt)->stdin_->streams);
        (*stmt)->stdin_->n = 0;
        (*stmt)->stdin_->streams = NULL;
        //free((*stmt)->stdin_);
        (*stmt)->stdin_ = NULL;

        // statement io redirection: stdout delete
        for (size_t i = 0; i < (*stmt)->stdout_->n; i++) {
                free((*stmt)->stdout_->streams[i]);
                (*stmt)->stdout_->streams[i] = NULL;
        }
        free((*stmt)->stdout_->streams);
        (*stmt)->stdout_->n = 0;
        (*stmt)->stdout_->streams = NULL;
        //free((*stmt)->stdout_);
        (*stmt)->stdout_ = NULL;

        // statement flags delete
        (*stmt)->flags = FLAGS_0;

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
void Statement_print(Statement const * const self)
{
        printf("STATEMENT(\n");
        printf("\tFLAGS(\n");
        if (self->flags == FLAGS_BGCTRL) {
                printf("\t\tBG_CTRL,\n");
        }
        printf("\t)\n");

        printf("\tCMD(\n");
        for (size_t i = 0; i < self->cmd->argc; i++) {
                printf("\t\tARG [%zu] = %s,\n", i, self->cmd->argv[i]);
        }
        printf("\t)\n");

        printf("\tSTDIN(\n");
        for (size_t i = 0; i < self->stdin_->n; i++) {
                printf("\t\t[%zu] = %s,\n", i, self->stdin_->streams[i]);
        }
        printf("\t)\n");

        printf("\tSTDOUT(\n");
        for (size_t i = 0; i < self->stdout_->n; i++) {
                printf("\t\t[%zu] = %s,\n", i, self->stdout_->streams[i]);
        }
        printf("\t)\n");
        printf(")\n");
}
