cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_C_STANDARD 99)

project(smallsh C)
set(PROJECT_VERSION "1.0.0")
set(PROJECT_DESCRIPTION "CS344, Operating Systems I, Assignment 3: smallsh")

set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -pedantic -g")

# Configure output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# Add subdirs
add_subdirectory(src)
add_subdirectory(examples)

# CMocka
include(cmake/cmocka.cmake)
include_directories(${CMOCKA_INCLUDE_DIR} include)
include(CTest)

# cmocka triggers c_flags so remove them for test build
unset(CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "-std=c99 -g -DTEST")
add_subdirectory(tests)