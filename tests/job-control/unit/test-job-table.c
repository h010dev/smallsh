/**
 * @file test-job-table.c
 * @author Mohamed Al-Hussein
 * @date 07 Feb 2022
 * @brief Unit test module for job-table.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "job-control/job-table.h"

struct JobTablePrivate {
        size_t jt_job_count;
        Job *jt_head;
};

static void job_table_test_ctor_initsTable(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // table data should be valid
        assert_non_null(table.private);
        assert_int_equal(0, table.private->jt_job_count);
        assert_null(table.private->jt_head);
}

static void job_table_test_add_job_emptyTable(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new process
        Process proc;
        process_ctor(&proc, 0, NULL, 1, false, 0);

        // create new job
        Job job;
        job_ctor(&job, &proc, 2, "abc.txt", "def.txt", false);

        // add job
        table.add_job(&table, &job);

        // job should be at head of table
        Job *head = table.private->jt_head;
        assert_non_null(head);

        // job data should be copied over
        assert_non_null(head->job_proc);
        assert_int_equal(1, head->job_proc->proc_pid);
        assert_non_null(head->job_proc->argv);
        assert_int_equal(false, head->job_proc->proc_completed);
        assert_int_equal(0, head->job_proc->proc_status);
        assert_int_equal(2, head->job_pgid);
        assert_string_equal("abc.txt", head->job_stdin);
        assert_string_equal("def.txt", head->job_stdout);
        assert_int_equal(false, head->job_bg);
        assert_null(head->job_next);

        // job count should be updated
        assert_int_equal(1, table.private->jt_job_count);
}

static void job_table_test_add_job_oneElemTable(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new processes
        Process proc1;
        process_ctor(&proc1, 0, NULL, 1, false, 0);

        Process proc2;
        process_ctor(&proc2, 0, NULL, 3, true, 2);

        // create new jobs
        Job job1;
        job_ctor(&job1, &proc1, 2, "abc.txt", "def.txt", false);

        Job job2;
        job_ctor(&job2, &proc2, 5, "ghi.txt", "jkl.txt", true);

        // add jobs
        table.add_job(&table, &job1);
        table.add_job(&table, &job2);

        // job2 should be at head of table
        Job *head = table.private->jt_head;
        assert_non_null(head);

        // job2 data should be copied over
        assert_non_null(head->job_proc);
        assert_int_equal(3, head->job_proc->proc_pid);
        assert_non_null(head->job_proc->argv);
        assert_int_equal(true, head->job_proc->proc_completed);
        assert_int_equal(2, head->job_proc->proc_status);
        assert_int_equal(5, head->job_pgid);
        assert_string_equal("ghi.txt", head->job_stdin);
        assert_string_equal("jkl.txt", head->job_stdout);
        assert_int_equal(true, head->job_bg);
        assert_non_null(head->job_next);

        // job1 should be at tail of table
        head = table.private->jt_head->job_next;
        assert_non_null(head);

        // job1 data should be copied over
        assert_non_null(head->job_proc);
        assert_int_equal(1, head->job_proc->proc_pid);
        assert_non_null(head->job_proc->argv);
        assert_int_equal(false, head->job_proc->proc_completed);
        assert_int_equal(0, head->job_proc->proc_status);
        assert_int_equal(2, head->job_pgid);
        assert_string_equal("abc.txt", head->job_stdin);
        assert_string_equal("def.txt", head->job_stdout);
        assert_int_equal(false, head->job_bg);
        assert_null(head->job_next);

        // job count should be updated
        assert_int_equal(2, table.private->jt_job_count);
}

static void job_table_clean_test_emptyTable(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // clean table
        table.clean(&table);

        // clean should do nothing
        assert_null(table.private->jt_head);
        assert_int_equal(0, table.private->jt_job_count);
}

static void job_table_clean_test_oneElemTableNoDirty(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new process
        Process proc;
        process_ctor(&proc, 0, NULL, 1, false, 0);

        // create new job
        Job job;
        job_ctor(&job, &proc, 2, "abc.txt", "def.txt", false);

        // add job
        table.add_job(&table, &job);

        // clean table
        table.clean(&table);

        // clean should do nothing
        assert_non_null(table.private->jt_head);
        assert_int_equal(1, table.private->jt_job_count);
}

static void job_table_clean_test_oneElemTableDirty(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new process
        Process proc;
        process_ctor(&proc, 0, NULL, 1, true, 0);

        // create new job
        Job job;
        job_ctor(&job, &proc, 2, "abc.txt", "def.txt", false);

        // add job
        table.add_job(&table, &job);

        // clean table
        table.clean(&table);

        // table should be empty
        assert_null(table.private->jt_head);
        assert_int_equal(0, table.private->jt_job_count);
}

static void job_table_clean_test_twoElemTableNoDirty(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new processes
        Process proc1;
        process_ctor(&proc1, 0, NULL, 1, false, 0);

        Process proc2;
        process_ctor(&proc2, 0, NULL, 3, false, 2);

        // create new jobs
        Job job1;
        job_ctor(&job1, &proc1, 2, "abc.txt", "def.txt", false);

        Job job2;
        job_ctor(&job2, &proc2, 5, "ghi.txt", "jkl.txt", false);

        // add jobs
        table.add_job(&table, &job1);
        table.add_job(&table, &job2);

        // clean table
        table.clean(&table);

        // clean should do nothing
        assert_non_null(table.private->jt_head);
        assert_non_null(table.private->jt_head->job_next);
        assert_int_equal(2, table.private->jt_job_count);
}

static void job_table_clean_test_twoElemTableLastDirty(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new processes
        Process proc1;
        process_ctor(&proc1, 0, NULL, 1, false, 0);

        Process proc2;
        process_ctor(&proc2, 0, NULL, 3, true, 2);

        // create new jobs
        Job job1;
        job_ctor(&job1, &proc1, 2, "abc.txt", "def.txt", false);

        Job job2;
        job_ctor(&job2, &proc2, 5, "ghi.txt", "jkl.txt", false);

        // add jobs
        table.add_job(&table, &job1);
        table.add_job(&table, &job2);

        // clean table
        table.clean(&table);

        // clean should remove tail
        assert_non_null(table.private->jt_head);
        assert_null(table.private->jt_head->job_next);
        assert_string_equal("abc.txt", table.private->jt_head->job_stdin);
        assert_int_equal(1, table.private->jt_job_count);
}

static void job_table_clean_test_twoElemTableFirstDirty(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new processes
        Process proc1;
        process_ctor(&proc1, 0, NULL, 1, false, 0);

        Process proc2;
        process_ctor(&proc2, 0, NULL, 3, true, 2);

        // create new jobs
        Job job1;
        job_ctor(&job1, &proc1, 2, "abc.txt", "def.txt", false);

        Job job2;
        job_ctor(&job2, &proc2, 5, "ghi.txt", "jkl.txt", false);

        // add jobs
        table.add_job(&table, &job2);
        table.add_job(&table, &job1);

        // clean table
        table.clean(&table);

        // clean should remove head
        assert_non_null(table.private->jt_head);
        assert_null(table.private->jt_head->job_next);
        assert_string_equal("abc.txt", table.private->jt_head->job_stdin);
        assert_int_equal(1, table.private->jt_job_count);
}

static void job_table_clean_test_twoElemTableBothDirty(void **state)
{
        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new processes
        Process proc1;
        process_ctor(&proc1, 0, NULL, 1, true, 0);

        Process proc2;
        process_ctor(&proc2, 0, NULL, 3, true, 2);

        // create new jobs
        Job job1;
        job_ctor(&job1, &proc1, 2, "abc.txt", "def.txt", false);

        Job job2;
        job_ctor(&job2, &proc2, 5, "ghi.txt", "jkl.txt", false);

        // add jobs
        table.add_job(&table, &job1);
        table.add_job(&table, &job2);

        // clean table
        table.clean(&table);

        // clean should empty table
        assert_null(table.private->jt_head);
        assert_int_equal(0, table.private->jt_job_count);
}

static void job_table_clean_test_cleanAfterEachAdd01(void **state)
{

        (void) state;

        // create new table
        struct JobTable table;
        job_table_ctor(&table);

        // create new process
        Process proc;
        process_ctor(&proc, 0, NULL, 1, true, 0);

        // create new job
        Job job;
        job_ctor(&job, &proc, 2, "abc.txt", "def.txt", false);

        // add job
        table.add_job(&table, &job);

        // clean table
        table.clean(&table);

        // table should be empty
        assert_null(table.private->jt_head);
        assert_int_equal(0, table.private->jt_job_count);

        ////////////////////////////////////////////////////

        // add job
        Process proc2;
        process_ctor(&proc2, 0, NULL, 3, true, 2);

        // create new job
        Job job2;
        job_ctor(&job2, &proc2, 5, "ghi.txt", "jkl.txt", false);

        // add job
        table.add_job(&table, &job2);

        // clean table
        table.clean(&table);

        // table should be empty
        assert_null(table.private->jt_head);
        assert_int_equal(0, table.private->jt_job_count);
}

static void job_table_clean_test_threeElemTableMidDirty(void **state)
{
        (void) state;
}

static void job_table_clean_test_threeElemTableEndsDirty(void **state)
{
        (void) state;
}

static void job_table_test_dtor_deletesTable(void **state)
{
        (void) state;
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        job_table_test_ctor_initsTable, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_test_add_job_emptyTable, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_test_add_job_oneElemTable, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_emptyTable, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_oneElemTableNoDirty, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_oneElemTableDirty, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_twoElemTableNoDirty, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_twoElemTableFirstDirty, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_twoElemTableLastDirty, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_twoElemTableBothDirty, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        job_table_clean_test_cleanAfterEachAdd01, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
