/**
 * @file shell-tokens.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief For parsing and storing the different types of tokens of the shell.
 */
#include <stdio.h>
#include <stdlib.h>

#include "interpreter/shell-tokens.h"

/* *****************************************************************************
 * PRIVATE DEFINITIONS
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
 * CLASS METHODS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/**
 * @brief Implementation of @c BGControlToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c BGControlToken object
 */
static void bgctrl_token_print_(Token const * const self)
{
        BGCtrlToken const * const self_ = (BGCtrlToken const *) self;

        printf("BG_CONTROL:%s", self_->super.vptr->get_value(self));
}

/**
 * @brief Implementation of @c BGControlToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed a background command token.
 * @param self pointer to @c BGControlToken object
 * @param iter pointer to @c StringIterator object
 */
static void bgctrl_token_take_(Token const * const self,
                               StringIterator const * const iter)
{
        BGCtrlToken const * const self_ = (BGCtrlToken const *) self;

        // copy background command symbol into token
        char *slice = iter->vptr->munch_char(iter);
        self_->super.vptr->set_value(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c CommentToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c CommentToken object
 */
static void comment_token_print_(Token const * const self)
{
        CommentToken const * const self_ = (CommentToken const *) self;

        printf("COMMENT:%s", self_->super.vptr->get_value(self));
}

/**
 * @brief Implementation of @c CommentToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed a comment token.
 * @param self pointer to @c CommentToken object
 * @param iter pointer to @c StringIterator object
 */
static void comment_token_take_(Token const * const self,
                                StringIterator const * const iter)
{
        CommentToken const * const self_ = (CommentToken const *) self;

        // copy comment symbol into token
        char *slice = iter->vptr->munch_char(iter);
        self_->super.vptr->set_value(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c InputRedirToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c InputRedirToken object
 */
static void inputredir_token_print_(Token const * const self)
{
        InputRedirToken const * const self_ = (InputRedirToken const *) self;

        printf("INPUT_REDIR:%s", self_->super.vptr->get_value(self));
}

/**
 * @brief Implementation of @c InputRedirToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed an input redirection token.
 * @param self pointer to @c InputRedirToken object
 * @param iter pointer to @c StringIterator object
 */
static void inputredir_token_take_(Token const * const self,
                                   StringIterator const * const iter)
{
        InputRedirToken const * const self_ = (InputRedirToken const *) self;

        // copy input redirection symbol into token
        char *slice = iter->vptr->munch_char(iter);
        self_->super.vptr->set_value(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c NewlineToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c NewlineToken object
 */
static void newline_token_print_(Token const * const self)
{
        (void) self;
        printf("NEWLINE:\\n");
}

/**
 * @brief Implementation of @c NewlineToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed an output redirection token.
 * @param self pointer to @c NewlineToken object
 * @param iter pointer to @c StringIterator object
 */
static void newline_token_take_(Token const * const self,
                                StringIterator const * const iter)
{
        NewlineToken const * const self_ = (NewlineToken const *) self;

        // copy output redirection symbol into token
        char *slice = iter->vptr->munch_char(iter);
        self_->super.vptr->set_value(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c OutputRedirToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c OutputRedirToken object
 */
static void outputredir_token_print_(Token const * const self)
{
        OutputRedirToken const * const self_ = (OutputRedirToken const *) self;

        printf("OUTPUT_REDIR:%s", self_->super.vptr->get_value(self));
}

/**
 * @brief Implementation of @c OutputRedirToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed an output redirection token.
 * @param self pointer to @c OutputRedirToken object
 * @param iter pointer to @c StringIterator object
 */
static void outputredir_token_take_(Token const * const self,
                                    StringIterator const * const iter)
{
        OutputRedirToken const * const self_ = (OutputRedirToken const *) self;

        // copy output redirection symbol into token
        char *slice = iter->vptr->munch_char(iter);
        self_->super.vptr->set_value(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c WordToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c WordToken object
 */
static void word_token_print_(Token const * const self)
{
        WordToken const * const self_ = (WordToken const *) self;

        printf("WORD:%s", self_->super.vptr->get_value(self));
}

/**
 * @brief Implementation of @c WordToken::Token::take().
 *
 * Iterates over a single word and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed a command token.
 * @param self pointer to @c WordToken object
 * @param iter pointer to @c StringIterator object
 */
static void word_token_take_(Token const * const self,
                             StringIterator const * const iter)
{
        WordToken const * const self_ = (WordToken const *) self;

        // copy word into token
        char *slice = iter->vptr->munch_word(iter);
        self_->super.vptr->set_value(self, slice);
        free(slice);
}

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
void bgctrl_token_ctor(BGCtrlToken *self)
{
        static struct TokenVtbl const vtbl = {
                .get_type = &token_get_type_,
                .get_value = &token_get_value_,
                .print = &bgctrl_token_print_,
                .set_value = &token_set_value_,
                .take = &bgctrl_token_take_,
        };
        token_ctor(&self->super, (TokenType) TOK_CTRL_BG);
        self->super.vptr = &vtbl;
}

void bgctrl_token_dtor(BGCtrlToken *self)
{
        token_dtor(&self->super);
}

void comment_token_ctor(CommentToken *self)
{
        static struct TokenVtbl const vtbl = {
                .get_type = &token_get_type_,
                .get_value = &token_get_value_,
                .print = &comment_token_print_,
                .set_value = &token_set_value_,
                .take = &comment_token_take_,
        };
        token_ctor(&self->super, (TokenType) TOK_CMT);
        self->super.vptr = &vtbl;
}

void comment_token_dtor(CommentToken *self)
{
        token_dtor(&self->super);
}

void inputredir_token_ctor(InputRedirToken *self)
{
        static struct TokenVtbl const vtbl = {
                .get_type = &token_get_type_,
                .get_value = &token_get_value_,
                .print = &inputredir_token_print_,
                .set_value = &token_set_value_,
                .take = &inputredir_token_take_,
        };
        token_ctor(&self->super, (TokenType) TOK_REDIR_INPUT);
        self->super.vptr = &vtbl;
}

void inputredir_token_dtor(InputRedirToken *self)
{
        token_dtor(&self->super);
}

void newline_token_ctor(NewlineToken *self)
{
        static struct TokenVtbl const vtbl = {
                .get_type = &token_get_type_,
                .get_value = &token_get_value_,
                .print = &newline_token_print_,
                .set_value = &token_set_value_,
                .take = &newline_token_take_,
        };
        token_ctor(&self->super, (TokenType) TOK_CTRL_NEWLINE);
        self->super.vptr = &vtbl;
}

void newline_token_dtor(NewlineToken *self)
{
        token_dtor(&self->super);
}

void outputredir_token_ctor(OutputRedirToken *self)
{
        static struct TokenVtbl const vtbl = {
                .get_type = &token_get_type_,
                .get_value = &token_get_value_,
                .print = &outputredir_token_print_,
                .set_value = &token_set_value_,
                .take = &outputredir_token_take_,
        };
        token_ctor(&self->super, (TokenType) TOK_REDIR_OUTPUT);
        self->super.vptr = &vtbl;
}

void outputredir_token_dtor(OutputRedirToken *self)
{
        token_dtor(&self->super);
}

void word_token_ctor(WordToken *self)
{
        static struct TokenVtbl const vtbl = {
                .get_type = &token_get_type_,
                .get_value = &token_get_value_,
                .print = &word_token_print_,
                .set_value = &token_set_value_,
                .take = &word_token_take_,
        };
        token_ctor(&self->super, (TokenType) TOK_WORD);
        self->super.vptr = &vtbl;
}

void word_token_dtor(WordToken *self)
{
        token_dtor(&self->super);
}
