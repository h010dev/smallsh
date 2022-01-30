enable_testing()

set(
        UTILS_UNITTEST_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/utils/string-iterator.c"
)

set(
        UTILS_UNITTESTS
        string-iterator
)

foreach(unit IN LISTS UTILS_UNITTESTS)
    add_executable(
            "utils-test-${unit}"
            "test-${unit}.c"
            ${UTILS_UNITTEST_SOURCE_FILES}
    )
    target_include_directories(
            "utils-test-${unit}"
            PRIVATE
            "${PROJECT_SOURCE_DIR}/include"
    )
    target_link_libraries("utils-test-${unit}" cmocka)
    add_test(
            NAME "utils-test-${unit}"
            COMMAND "utils-test-${unit}"
    )
endforeach()
