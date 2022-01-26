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
        -o smallsh \
        src/main.c \
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