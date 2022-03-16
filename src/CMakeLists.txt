set(
        SH_SOURCE_FILES
        smallsh.c
        error.c

        builtins/builtins.c
        builtins/cd.c
        builtins/status.c
        builtins/exit.c

        events/events.c
        events/sender.c
        events/receiver.c
        events/channel.c

        interpreter/parser.c
        interpreter/statement.c
        interpreter/token-iterator.c
        interpreter/lexer.c
        interpreter/shell-tokens.c
        interpreter/token.c

        job-control/job-control.c
        job-control/job-table.c
        job-control/job.c
        job-control/process.c

        signals/installer.c
        signals/handler.c

        utils/string-iterator.c
)

set(
        SH_TARGETS
        ${PROJECT_NAME}
)

foreach(target IN LISTS SH_TARGETS)
    add_executable(
            ${target}
            ${SH_${target}_SOURCE_FILES}
            ${SH_SOURCE_FILES}
    )
    target_include_directories(
            ${target}
            PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )
endforeach()
