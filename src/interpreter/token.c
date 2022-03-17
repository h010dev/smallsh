/**
 * @file token.c
 * @author Mohamed Al-Hussein
 * @date 25 Jan 2022
 * @brief For parsing and storing tokens.
 *
 * This is a base class for parsing strings into categorized tokens. This class
 * is not meant to be used on its own, rather it should be inherited and
 * implemented into more specific token classes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter/token.h"
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
 * CONSTRUCTORS + DESTRUCTORS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
SH_Token *SH_CreateToken(SH_TokenType type)
{
        SH_Token *token;

        token = malloc(sizeof *token);
        if (token == NULL) {
                return NULL;
        }

        token->type = type;
        token->value = NULL;

        return token;
}

void SH_DestroyToken(SH_Token **token)
{
        if (*token) {
                (*token)->type = TOK_0;

                free((*token)->value);
                (*token)->value = NULL;

                free(*token);

                *token = NULL;
        }
}
/* *****************************************************************************
 * GETTERS/SETTERS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void SH_SetTokenValue(SH_Token * const token, char const *value)
{
        // free original value before re-initializing it with new data
        if (token->value != NULL) {
                free(token->value);
                token->value = NULL;
        }

        token->value = strdup(value);
}
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
void SH_PrintToken(SH_Token const * const token)
{
        switch (token->type) {
                case TOK_CTRL_BG:
                        printf("BG_CONTROL:%s", token->value);
                        break;
                case TOK_CMT:
                        printf("COMMENT:%s", token->value);
                        break;
                case TOK_REDIR_INPUT:
                        printf("INPUT_REDIR:%s", token->value);
                        break;
                case TOK_CTRL_NEWLINE:
                        printf("NEWLINE:\\n");
                        break;
                case TOK_REDIR_OUTPUT:
                        printf("OUTPUT_REDIR:%s", token->value);
                        break;
                case TOK_WORD:
                        printf("WORD:%s", token->value);
                        break;
                default:
                        break;
        }
}

void SH_TakeToken(SH_Token *const token, SH_StringIterator *const iter)
{
        switch(token->type) {
                case TOK_CTRL_BG:
                case TOK_CMT:
                case TOK_REDIR_INPUT:
                case TOK_REDIR_OUTPUT:
                case TOK_CTRL_NEWLINE:
                {
                        char *slice = SH_StringIteratorConsumeChar(iter);
                        SH_SetTokenValue(token, slice);
                        free(slice);
                        break;
                }
                case TOK_WORD:
                {
                        char *slice = SH_StringIteratorConsumeWord(iter);
                        SH_SetTokenValue(token, slice);
                        free(slice);
                        break;
                }
                default:
                        break;
        }
}
