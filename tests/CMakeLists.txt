#################### CREATE EXECUTABLES ####################
# TEST-MAIN
add_executable(
        test-main
        test-main.c
)
target_link_libraries(test-main cmocka)

# TEST-STRING-ITERATOR
add_executable(
        test-string-iterator
        test-string-iterator.c
)
target_include_directories(
        test-string-iterator
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries(test-string-iterator cmocka)

#################### ADD TESTS ####################
enable_testing()
add_test(
        NAME test-main
        COMMAND test-main
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

add_test(
        NAME test-string-iterator
        COMMAND test-string-iterator
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)
