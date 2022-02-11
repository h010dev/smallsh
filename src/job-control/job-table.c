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
/**
 * @brief Contains internal data for @c JobTable.
 */
struct JobTablePrivate {
        size_t jt_job_count; /**< number of jobs in table */
        Job *jt_head; /**< pointer to linked list head */
};

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
static void job_table_add_job_(struct JobTable const * const self, Job *job)
{
        struct JobTablePrivate *priv = self->private;

        /* Case 1: Table is empty. */
        if (priv->jt_head == NULL) {
                priv->jt_head = job;
                job->job_spec = 1;
        }
        /* Case 2: Table contains at least one job. */
        else {
                Job *head = priv->jt_head;
                job->job_next = head;
                job->job_spec = head->job_spec + 1;
                priv->jt_head = job;
        }
        priv->jt_job_count++;
}

static void job_table_clean_(struct JobTable const * const self)
{
        struct JobTablePrivate *priv = self->private;

        /* Track last and second last jobs for display options. */
        unsigned last_spec_01 = 0;
        unsigned last_spec_02 = 0;

        Job *pre = NULL;
        Job *cur = priv->jt_head;
        while (cur != NULL) {
                /* Mark last and second last jobs for print display. */
                if (last_spec_01 == 0) {
                        last_spec_01 = cur->job_spec;
                } else if (last_spec_02 == 0) {
                        last_spec_02 = cur->job_spec;
                }

                /* Remove completed job and print info. */
                if (cur->job_proc->proc_completed) {
                        /* Background job completed; notify user. */
                        if (cur->job_bg) {
                                /* Print job info. */
                                printf("[%d]", cur->job_spec);
                                if (cur->job_spec == last_spec_01) {
                                        printf("+");
                                } else if (cur->job_spec == last_spec_02) {
                                        printf("-");
                                }

                                printf("\t%d", cur->job_proc->proc_pid);

                                printf("\tDone");

                                if (cur->job_proc->proc_status == 0) {
                                        printf("\tEXIT 0");
                                } else {
                                        printf("\tTERM %d",
                                               cur->job_proc->proc_status);
                                }

                                printf("\t\t%s\n", cur->job_proc->argv[0]);
                        }

                        /* Remove job and free its memory. */
                        /* Case 1: Job was at list head. */
                        if (cur == priv->jt_head) {
                                Job *head = priv->jt_head;
                                priv->jt_head = head->job_next;

                                job_dtor(head);
                                free(head);
                                head = NULL;

                                cur = priv->jt_head;
                        }
                        /* Case 2: Job was n positions after list head. */
                        else {
                                Job *old = cur;
                                pre->job_next = cur->job_next;
                                cur = pre->job_next;

                                job_dtor(old);
                                free(old);
                                old = NULL;
                        }
                        priv->jt_job_count--;
                }
                /* Job is not yet completed. */
                else {
                        pre = cur;
                        cur = cur->job_next;
                }
        }
}

static Job *job_table_find_job_(struct JobTable const * const self,
        pid_t job_pgid)
{
        struct JobTablePrivate *priv = self->private;

        /* Search all entries for job with matching PGID. */
        Job *job = priv->jt_head;
        while (job != NULL) {
                if (job->job_pgid == job_pgid) {
                        return job;
                }
                job = job->job_next;
        }

        /* Job not found. */
        return NULL;
}

static void job_table_killall_(struct JobTable const * const self)
{
        struct JobTablePrivate *priv = self->private;

        Job *job = priv->jt_head;

        while (job != NULL) {
                pid_t pid = job->job_proc->proc_pid;
                kill(pid, SIGTERM);
                job = job->job_next;
        }

        self->clean(self);
}

static void job_table_list_jobs_(struct JobTable const * const self)
{
        struct JobTablePrivate *priv = self->private;

        Job *job = priv->jt_head;
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
                       job->job_pgid, job->job_stdin,
                       job->job_stdout, job->job_spec, job->job_proc->argv[0],
                       job->job_proc->proc_pid, job->job_proc->proc_completed,
                       job->job_proc->proc_status);
                job = job->job_next;
        }
}

static int job_table_update_(struct JobTable const * const self, pid_t pid,
        int status)
{
        struct JobTablePrivate *priv = self->private;

        /* Find job with matching PID. */
        Job *job = priv->jt_head;
        while (job != NULL) {
                if (job->job_proc->proc_pid == pid) {
                        break;
                }
                job = job->job_next;
        }

        /* Error! Job not found. */
        if (job == NULL) {
                return -1;
        }

        /* Update job status. */
        job->job_proc->proc_status = status;
        job->job_proc->proc_completed = true;

        return 0;
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
 * CONSTRUCTORS + DESTRUCTORS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void job_table_ctor(JobTable *self)
{
        /* Init table functions. */
        self->add_job = &job_table_add_job_;
        self->clean = &job_table_clean_;
        self->find_job = &job_table_find_job_;
        self->killall = &job_table_killall_;
        self->list_jobs = &job_table_list_jobs_;
        self->update = &job_table_update_;

        /* Init table data. */
        self->private = malloc(sizeof(struct JobTablePrivate));
        self->private->jt_job_count = 0;
        self->private->jt_head = NULL;
}

void job_table_dtor(JobTable *self)
{
        struct JobTablePrivate *priv = self->private;

        /* Free all jobs. */
        while (priv->jt_head != NULL) {
                Job *head = priv->jt_head;
                priv->jt_head = head->job_next;
                job_dtor(head);
                free(head);
                head = NULL;
        }
        self->private->jt_job_count = 0;

        /* Reset function pointers. */
        self->add_job = NULL;
        self->clean = NULL;
        self->find_job = NULL;
        self->killall = NULL;
        self->list_jobs = NULL;
        self->update = NULL;

        /* Free table data. */
        free(self->private);
        self->private = NULL;
}
