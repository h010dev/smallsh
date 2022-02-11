/**
 * @file error.c
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief For recording and responding to shell errors.
 */
#ifdef DEBUG
#include <stdio.h>
#endif

#include "error.h"

/* *****************************************************************************
 * PUBLIC DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void print_error_msg(const char *msg)
{
        (void) msg;

#ifdef DEBUG
        fprintf(stderr, "%s\n", msg);
        fflush(stderr);
#endif
}
