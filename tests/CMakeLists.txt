enable_testing()

##################### TEST-MAIN #####################
add_executable(
        test-main
        test-main.c
)
target_link_libraries(test-main cmocka)

add_test(
        NAME test-main
        COMMAND test-main
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

##################### TEST-STRING-ITERATOR #####################
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

add_test(
        NAME test-string-iterator
        COMMAND test-string-iterator
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

##################### TEST-TOKEN #####################
add_executable(
        test-token
        test-token.c
)
target_include_directories(
        test-token
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries(test-token cmocka)

add_test(
        NAME test-token
        COMMAND test-token
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

##################### TEST-SHELL-TOKENS #####################
add_executable(
        "test-shell-tokens"
        test-shell-tokens.c
        "${PROJECT_SOURCE_DIR}/src/string-iterator.c"
        "${PROJECT_SOURCE_DIR}/src/shell-tokens.c"
        "${PROJECT_SOURCE_DIR}/src/token.c"
)
target_include_directories(
        "test-shell-tokens"
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries("test-shell-tokens" cmocka)

add_test(
        NAME "test-shell-tokens"
        COMMAND "test-shell-tokens"
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

##################### TEST-LEXER #####################
add_executable(
        "test-lexer"
        test-lexer.c
        "${PROJECT_SOURCE_DIR}/src/lexer.c"
        "${PROJECT_SOURCE_DIR}/src/string-iterator.c"
        "${PROJECT_SOURCE_DIR}/src/shell-tokens.c"
        "${PROJECT_SOURCE_DIR}/src/token.c"
)
target_include_directories(
        "test-lexer"
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries("test-lexer" cmocka)

add_test(
        NAME "test-lexer"
        COMMAND "test-lexer"
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

##################### TEST-TOKEN-ITERATOR ####################
add_executable(
        "test-token-iterator"
        test-token-iterator.c
        "${PROJECT_SOURCE_DIR}/src/node-stack.c"
        "${PROJECT_SOURCE_DIR}/src/node.c"
        "${PROJECT_SOURCE_DIR}/src/lexer.c"
        "${PROJECT_SOURCE_DIR}/src/string-iterator.c"
        "${PROJECT_SOURCE_DIR}/src/shell-tokens.c"
        "${PROJECT_SOURCE_DIR}/src/token.c"
)
target_include_directories(
        "test-token-iterator"
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries("test-token-iterator" cmocka)

add_test(
        NAME "test-token-iterator"
        COMMAND "test-token-iterator"
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

##################### TEST-NODE ####################
add_executable(
        test-node
        test-node.c
        "${PROJECT_SOURCE_DIR}/src/shell-tokens.c"
        "${PROJECT_SOURCE_DIR}/src/token.c"
)
target_include_directories(
        test-node
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries(test-node cmocka)

add_test(
        NAME test-node
        COMMAND test-node
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)

##################### TEST-NODE-STACK ####################
add_executable(
        test-node-stack
        test-node-stack.c
        "${PROJECT_SOURCE_DIR}/src/node.c"
)
target_include_directories(
        test-node-stack
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries(test-node-stack cmocka)

add_test(
        NAME test-node-stack
        COMMAND test-node-stack
        WORKING_DIRECTORY /nfs/stak/users/alhussem/cs344/a3
)
