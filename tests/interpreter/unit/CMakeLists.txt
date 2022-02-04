enable_testing()

set(
        PARSER_UNITTEST_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/interpreter/lexer.c"
        "${PROJECT_SOURCE_DIR}/src/interpreter/parser.c"
        "${PROJECT_SOURCE_DIR}/src/interpreter/shell-tokens.c"
        "${PROJECT_SOURCE_DIR}/src/interpreter/statement.c"
        "${PROJECT_SOURCE_DIR}/src/interpreter/token.c"
        "${PROJECT_SOURCE_DIR}/src/interpreter/token-iterator.c"
        "${PROJECT_SOURCE_DIR}/src/utils/string-iterator.c"
)

set(
        PARSER_UNITTESTS
        token
        shell-tokens
        lexer
        token-iterator
        parser
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
