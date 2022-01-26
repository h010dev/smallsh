#################### CREATE EXECUTABLES ####################
# TEST-MAIN
add_executable(
        test-main
        test-main.c
)
target_link_libraries(test-main cmocka)

#################### ADD TESTS ####################
enable_testing()
add_test(
        NAME test-main
        COMMAND test-main
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)