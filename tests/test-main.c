#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

static void null_test_success(void **state)
{
        (void) state;
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        null_test_success,
                        NULL,
                        NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
