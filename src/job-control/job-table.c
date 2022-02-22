/**
 * @file job-table.c
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contain information about a collection of jobs.
 *
 * Ideas presented here were retrieved from the following source:
 * https://www.gnu.org/software/libc/manual/html_node/Data-Structures.html
 */
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "job-control/job-table.h"

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
SH_JobTable *SH_CreateJobTable(void)
{
        SH_JobTable *table;

        table = malloc(sizeof *table);
        if (table == NULL) {
                fprintf(stderr, "malloc\n");
                exit(1);
        }

        /* Init table data. */
        table->n_jobs = 0;
        table->head = NULL;

        return table;
}

void SH_DestroyJobTable(SH_JobTable *table)
{
        /* Free all jobs. */
        while (table->head != NULL) {
                SH_Job *head = table->head;
                table->head = head->next;
                SH_DestroyJob(head);
                head = NULL;
        }
        table->n_jobs = 0;

        free(table);
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
void SH_JobTableAddJob(SH_JobTable *table, SH_Job *job)
{
        /* Case 1: Table is empty. */
        if (table->head == NULL) {
                table->head = job;
                job->spec = 1;
        }
                /* Case 2: Table contains at least one job. */
        else {
                SH_Job *head = table->head;
                job->next = head;
                job->spec = head->spec + 1;
                table->head = job;
        }
        table->n_jobs++;
}

void SH_JobTableCleanJobs(SH_JobTable *table)
{
        /* Track last and second last jobs for display options. */
        unsigned last_spec_01 = 0;
        unsigned last_spec_02 = 0;

        SH_Job *pre = NULL;
        SH_Job *cur = table->head;
        while (cur != NULL) {
                /* Mark last and second last jobs for print display. */
                if (last_spec_01 == 0) {
                        last_spec_01 = cur->spec;
                } else if (last_spec_02 == 0) {
                        last_spec_02 = cur->spec;
                }

                /* Remove completed job and print info. */
                if (cur->proc->has_completed) {
                        /* Background job completed; notify user. */
                        if (cur->run_bg) {
                                /* Print job info. */
                                fprintf(stdout, "[%d]", cur->spec);
                                if (cur->spec == last_spec_01) {
                                        fprintf(stdout, "+");
                                } else if (cur->spec == last_spec_02) {
                                        fprintf(stdout, "-");
                                }

                                fprintf(stdout, "\t%d", cur->proc->pid);

                                fprintf(stdout, "\tDone");

                                if (cur->proc->status == 0) {
                                        fprintf(stdout, "\t\texit value 0");
                                } else {
                                        fprintf(stdout, "\t\tterminated by signal %d",
                                                cur->proc->status);
                                }

                                fprintf(stdout, "\t\t%s\n", cur->command);
                                fflush(stdout);
                        }

                        /* Remove job and free its memory. */
                        /* Case 1: Job was at list head. */
                        if (cur == table->head) {
                                SH_Job *head = table->head;
                                table->head = head->next;

                                SH_DestroyJob(head);
                                head = NULL;

                                cur = table->head;
                        }
                                /* Case 2: Job was n positions after list head. */
                        else {
                                SH_Job *old = cur;
                                pre->next = cur->next;
                                cur = pre->next;

                                SH_DestroyJob(old);
                                old = NULL;
                        }
                        table->n_jobs--;
                }
                        /* Job is not yet completed. */
                else {
                        pre = cur;
                        cur = cur->next;
                }
        }
}

SH_Job *SH_JobTableFindJob(const SH_JobTable *table, pid_t job_pgid)
{
        /* Search all entries for job with matching PGID. */
        SH_Job *job = table->head;
        while (job != NULL) {
                if (job->pgid == job_pgid) {
                        return job;
                }
                job = job->next;
        }

        /* Job not found. */
        return NULL;
}

void SH_JobTableKillAllJobs(SH_JobTable *table)
{
        SH_Job *job = table->head;

        while (job != NULL) {
                pid_t pid = job->proc->pid;
                kill(pid, SIGTERM);
                job = job->next;
        }

        SH_JobTableCleanJobs(table);
}

void SH_JobTablePrintJobs(const SH_JobTable *table)
{
        SH_Job *job = table->head;
        while (job != NULL) {
                printf("JOB:\n"
                       "\tpgid=%d\n"
                       "\tstdin=%s\n"
                       "\tstdout=%s\n"
                       "\tspec=%d\n"
                       "\tPROC:\n"
                       "\t\targv[0]=%s\n"
                       "\t\tpid=%d\n"
                       "\t\tcompleted=%d\n"
                       "\t\tstatus=%d\n",
                       job->pgid, job->infile,
                       job->outfile, job->spec, job->proc->args[0],
                       job->proc->pid, job->proc->has_completed,
                       job->proc->status);
                job = job->next;
        }
}

int SH_JobTableUpdateJob(const SH_JobTable *table, pid_t pid, int status)
{
        /* Find job with matching PID. */
        SH_Job *job = table->head;
        while (job != NULL) {
                if (job->proc->pid == pid) {
                        break;
                }
                job = job->next;
        }

        /* Error! Job not found. */
        if (job == NULL) {
                return -1;
        }

        /* Update job status. */
        job->proc->status = status;
        job->proc->has_completed = true;

        return 0;
}
