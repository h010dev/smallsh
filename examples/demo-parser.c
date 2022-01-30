/**
 * @file demo-parser.c
 * @authoer Mohamed Al-Hussein
 * @date 29 Jan 2022
 * @brief Simple interactive shell for demoing the parser.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/parser.h"
#include "parser/lexer.h"
#include "parser/node.h"

int main(void)
{
        while (1) {
                ssize_t n_read;
                size_t len = 0;
                char *line = NULL;
                char *exit_msg = "exit\n";

                // shell prompt
                if(fputs(": ", stdout) == EOF) {
                        // error
                        exit(EXIT_FAILURE);
                }

                // read input
                errno = 0;
                n_read = getline(&line, &len, stdin);
                if (n_read == -1) {
                        // error
                        exit(EXIT_FAILURE);
                }

                // exit shell
                if (strcmp(exit_msg, line) == 0) {
                        free(line);
                        exit(EXIT_SUCCESS);
                }

                Parser parser;
                Parser_ctor(&parser);

                parser.parse(&parser, line);
                parser.cleanup(&parser, true);

                Parser_dtor(&parser);

                free(line);
        }
}
