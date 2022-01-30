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

#include "parser.h"
#include "lexer.h"
#include "node.h"

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

                // parse stream into tokens
                Token **tokens = malloc(MAX_TOKENS * sizeof(Token));
                size_t n_tok = generate_tokens(line, MAX_TOKENS, tokens);

                // parse tokens into nodes
                Stack *stack = malloc(sizeof(Stack));
                Stack_ctor(stack, MAX_TOKENS);
                size_t n_node = parse(n_tok, tokens, stack);
                (void) n_node;

                // display parse tree to stdout
                while (!stack->isEmpty(stack)) {
                        Node *node = (Node *) stack->pop(stack);
                        node->vptr->print(node);

                        // cleanup
                        NodeValue *value = node->vptr->getValue(node);
                        NodeType type = node->vptr->getType(node);
                        if (type == NODE_CMD) {
                                CommandValue *cmd;
                                cmd = &value->cmd_value;
                                for (size_t i = 0; i < cmd->argc; i++) {
                                        cmd->argv[i] = NULL;
                                }
                                free(cmd->argv);
                                cmd->argv = NULL;
                        }
                        Node_dtor(node);
                        free(node);
                        node = NULL;
                }

                // cleanup
                for (size_t i = 0; i < n_tok; i++) {
                        Token_dtor(tokens[i]);
                        free(tokens[i]);
                }
                free(tokens);
                Stack_dtor(stack);
                free(stack);
                free(line);
        }
}
