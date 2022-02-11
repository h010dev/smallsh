/**
 * @file builtins.c
 * @author Mohamed Al-Hussein
 * @date 10 Feb 2022
 * @brief For identifying and invoking shell builtin commands.
 */
#include <stdbool.h>
#include <string.h>

#include "builtins/builtins.h"

/**
 * @brief Constants for defining supported builtins.
 */
enum {
        BUILTINS_CD, /**< cd command */
        BUILTINS_EXIT, /**< exit command */
        BUILTINS_STATUS, /**< status command */
        BUILTINS_COUNT, /**< number of supported builtins */
};

/**
 * @brief Translates builtin constants into their string representations.
 */
static const char * const builtins[] = {
        [BUILTINS_CD] = "cd",
        [BUILTINS_EXIT] = "exit",
        [BUILTINS_STATUS] = "status",
};

bool builtins_is_supported(char *cmd)
{
        /* Iterate over supported builtins looking for an exact match. */
        for (size_t i = 0; i < BUILTINS_COUNT; i++) {
                if (strcmp(builtins[i], cmd) == 0) {
                        return true;
                }
        }

        return false;
}
