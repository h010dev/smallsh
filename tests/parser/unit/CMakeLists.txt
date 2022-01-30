enable_testing()

set(
        PARSER_UNITTEST_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/parser/lexer.c"
        "${PROJECT_SOURCE_DIR}/src/parser/node.c"
        "${PROJECT_SOURCE_DIR}/src/parser/node-stack.c"
        "${PROJECT_SOURCE_DIR}/src/parser/parser.c"
        "${PROJECT_SOURCE_DIR}/src/parser/shell-tokens.c"
        "${PROJECT_SOURCE_DIR}/src/parser/token.c"
        "${PROJECT_SOURCE_DIR}/src/parser/token-iterator.c"
        "${PROJECT_SOURCE_DIR}/src/utils/string-iterator.c"
)

set(
        PARSER_UNITTESTS
        token
        shell-tokens
        lexer
        token-iterator
        node
        node-stack
        #parser
)

foreach(unit IN LISTS PARSER_UNITTESTS)
    add_executable(
            "parser-test-${unit}"
            "test-${unit}.c"
            ${PARSER_UNITTEST_SOURCE_FILES}
    )
    target_include_directories(
            "parser-test-${unit}"
            PRIVATE
            "${PROJECT_SOURCE_DIR}/include"
    )
    target_link_libraries("parser-test-${unit}" cmocka)
    add_test(
            NAME "parser-test-${unit}"
            COMMAND "parser-test-${unit}"
    )
endforeach()
