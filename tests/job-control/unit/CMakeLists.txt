enable_testing()

set(
        SOURCE_PREFIX
        JOB_CONTROL
)

set(
        ${SOURCE_PREFIX}_UNITTEST_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/job-control/job-control.c"
        "${PROJECT_SOURCE_DIR}/src/job-control/job.c"
        "${PROJECT_SOURCE_DIR}/src/job-control/process.c"
        "${PROJECT_SOURCE_DIR}/src/job-control/job-table.c"
        "${PROJECT_SOURCE_DIR}/src/signals/installer.c"
        "${PROJECT_SOURCE_DIR}/src/signals/handler.c"
        "${PROJECT_SOURCE_DIR}/src/signals/dispatcher.c"
)

set(
        JOB_CONTROL_UNITTESTS
        job-table
)

set(
        TEST_UNIT
        job-control
)

foreach(unit IN LISTS JOB_CONTROL_UNITTESTS)
    add_executable(
            "${TEST_UNIT}-test-${unit}"
            "test-${unit}.c"
            ${${SOURCE_PREFIX}_UNITTEST_SOURCE_FILES}
    )
    target_include_directories(
            "${TEST_UNIT}-test-${unit}"
            PRIVATE
            "${PROJECT_SOURCE_DIR}/include"
    )
    target_link_libraries("${TEST_UNIT}-test-${unit}" cmocka)
    add_test(
            NAME "${TEST_UNIT}-test-${unit}"
            COMMAND "${TEST_UNIT}-test-${unit}"
    )
endforeach()
