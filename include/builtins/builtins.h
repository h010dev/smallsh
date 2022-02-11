/**
 * @file builtins.h
 * @author Mohamed Al-Hussein
 * @date 10 Feb 2022
 * @brief For identifying and invoking shell builtin commands.
 */
#ifndef SMALLSH_BUILTINS_H
#define SMALLSH_BUILTINS_H

#include <stdbool.h>

#include "cd.h"
#include "exit.h"
#include "status.h"

/**
 * @brief Checks @p cmd against supported builtin commands.
 * @param cmd command string to check
 * @return true if @p cmd is a supported builtin, false otherwise
 */
bool builtins_is_supported(char *cmd);

#endif //SMALLSH_BUILTINS_H
