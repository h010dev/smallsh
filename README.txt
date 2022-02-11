# smallsh
Simple implementation of a Linux shell written in C.

### Build

#### Option 1: Use build script
```asm
chmod u+x ./build.sh && ./build.sh
```

#### Option 2: Link and build manually
```asm
mkdir -p build/bin && \
    gcc --std=c99 \
        -g -Wall -Wextra -Werror -pedantic \
        -DTEST_SCRIPT -DDEMO \
        -o smallsh \
        src/smallsh.c \
        src/error.c \
        src/builtins/builtins.c \
        src/builtins/cd.c \
        src/builtins/exit.c \
        src/builtins/status.c \
        src/events/channel.c \
        src/events/events.c \
        src/events/receiver.c \
        src/events/sender.c \
        src/interpreter/lexer.c \
        src/interpreter/parser.c \
        src/interpreter/shell-tokens.c \
        src/interpreter/statement.c \
        src/interpreter/token.c \
        src/interpreter/token-iterator.c \
        src/job-control/job.c \
        src/job-control/job-control.c \
        src/job-control/job-table.c \
        src/job-control/process.c \
        src/signals/handler.c \
        src/signals/installer.c \
        src/utils/string-iterator.c \
        -Iinclude/ && \
    mv ./smallsh build/bin
```

### Run
```asm
build/bin/smallsh
```

### Clean
```asm
rm -rf build
```
