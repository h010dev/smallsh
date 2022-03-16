/**
 * @file error.h
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief For recording and responding to shell errors.
 */
#ifndef SMALLSH_ERROR_H
#define SMALLSH_ERROR_H

extern volatile int smallsh_errno; /**< shell error SH_status code */

/**
 * @brief Prints an error message to STDOUT.
 * @param msg message to print
 */
void print_error_msg(const char *msg);

#endif //SMALLSH_ERROR_H
