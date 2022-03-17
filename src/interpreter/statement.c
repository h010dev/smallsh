/**
 * @file statement.c
 * @author Mohamed Al-Hussein
 * @date 02 Feb 2022
 * @brief For creating command statements to later be executed.
 */
#include <stdlib.h>
#include <stdio.h>

#include "interpreter/statement.h"
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
SH_Statement *SH_CreateStatement(void)
{
        SH_Statement *stmt = malloc(sizeof(SH_Statement));

        // statement command init
        stmt->cmd = malloc(sizeof(StmtCmd));
        stmt->cmd->count = 0;
        stmt->cmd->args = malloc(sizeof(char *));
        stmt->cmd->args[0] = NULL;

        // statement io redirection: stdin init
        stmt->infile = malloc(sizeof(StmtStdin));
        stmt->infile->n = 0;
        stmt->infile->streams = malloc(sizeof(char *));
        stmt->infile->streams[0] = NULL;

        // statement io redirection: stdout init
        stmt->outfile = malloc(sizeof(StmtStdout));
        stmt->outfile->n = 0;
        stmt->outfile->streams = malloc(sizeof(char *));
        stmt->outfile->streams[0] = NULL;

        // statement flags init
        stmt->flags = 0;

        return stmt;
}

void SH_DestroyStatement(SH_Statement **stmt)
{
        // statement command delete
        for (size_t i = 0; i < (*stmt)->cmd->count; i++) {
                free((*stmt)->cmd->args[i]);
                (*stmt)->cmd->args[i] = NULL;
        }
        free((*stmt)->cmd->args);
        (*stmt)->cmd->count = 0;
        (*stmt)->cmd->args = NULL;
        free((*stmt)->cmd);
        (*stmt)->cmd = NULL;

        // statement io redirection: stdin delete
        for (size_t i = 0; i < (*stmt)->infile->n; i++) {
                free((*stmt)->infile->streams[i]);
                (*stmt)->infile->streams[i] = NULL;
        }
        free((*stmt)->infile->streams);
        (*stmt)->infile->n = 0;
        (*stmt)->infile->streams = NULL;
        free((*stmt)->infile);
        (*stmt)->infile = NULL;

        // statement io redirection: stdout delete
        for (size_t i = 0; i < (*stmt)->outfile->n; i++) {
                free((*stmt)->outfile->streams[i]);
                (*stmt)->outfile->streams[i] = NULL;
        }
        free((*stmt)->outfile->streams);
        (*stmt)->outfile->n = 0;
        (*stmt)->outfile->streams = NULL;
        free((*stmt)->outfile);
        (*stmt)->outfile = NULL;

        // statement flags delete
        (*stmt)->flags = FLAGS_NONE;

        // statement delete
        free(*stmt);
        *stmt = NULL;
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
void SH_PrintStatement(SH_Statement const *stmt)
{
        printf("STATEMENT(\n");
        printf("\tFLAGS(\n");
        if (stmt->flags == FLAGS_BGCTRL) {
                printf("\t\tBG_CTRL,\n");
        }
        printf("\t)\n");

        printf("\tCMD(\n");
        for (size_t i = 0; i < stmt->cmd->count; i++) {
                printf("\t\tARG [%zu] = %s,\n", i, stmt->cmd->args[i]);
        }
        printf("\t)\n");

        printf("\tSTDIN(\n");
        for (size_t i = 0; i < stmt->infile->n; i++) {
                printf("\t\t[%zu] = %s,\n", i, stmt->infile->streams[i]);
        }
        printf("\t)\n");

        printf("\tSTDOUT(\n");
        for (size_t i = 0; i < stmt->outfile->n; i++) {
                printf("\t\t[%zu] = %s,\n", i, stmt->outfile->streams[i]);
        }
        printf("\t)\n");
        printf(")\n");
}
