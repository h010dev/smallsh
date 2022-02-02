#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/parser.h"
#include "shell/signal-handlers.h"

static int register_signals(void)
{
        if (signal(SIGINT, handle_SIGINT) == SIG_ERR) {
                fprintf(stderr, "signal(SIGINT, handle_SIGINT) failed\n");
                fflush(stderr);
                return -1;
        }
        if (signal(SIGTSTP, handle_SIGTSTP) == SIG_ERR) {
                fprintf(stderr, "signal(SIGTSTP, handle_SIGTSTP) failed\n");
                fflush(stderr);
                return -1;
        }
        return 0;
}

int smallsh_run(void)
{
        // register signals
        if (register_signals() == -1) {
                fprintf(stderr, "register_signals() failed\n");
                fflush(stderr);
                return EXIT_FAILURE;
        }

        // run event loop
        while (1) {
                ssize_t n_read;
                size_t len = 0;
                char *line = NULL;
                char *exit_msg = "exit\n";

                // shell prompt
                if(fputs(": ", stdout) == EOF) {
                        // error
                        return EXIT_FAILURE;
                }

                // read input
                errno = 0;
                n_read = getline(&line, &len, stdin);
                if (n_read == -1) {
                        // error
                        return EXIT_FAILURE;
                }

                // exit shell
                if (strcmp(exit_msg, line) == 0) {
                        free(line);
                        return EXIT_SUCCESS;
                }

                Parser parser;
                Parser_ctor(&parser);

                ssize_t n_stmts = parser.parse(&parser, line);
                if (n_stmts == -1) {
                        return EXIT_FAILURE;
                }
                Statement **stmts = parser.get_statements(&parser);

                for (ssize_t i = 0; i < n_stmts; i++) {
                        parser.print_statement(stmts[i]);
                        Statement_del(&stmts[i]);
                }

                Parser_dtor(&parser);

                free(line);
        }
}
