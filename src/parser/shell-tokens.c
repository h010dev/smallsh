/**
 * @file shell-tokens.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief For parsing and storing the different types of tokens of the shell.
 */
#include <stdio.h>
#include <stdlib.h>

#include "parser/shell-tokens.h"

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
static void BGControlToken_print_(Token const * const self)
{
        BGControlToken const * const self_ = (BGControlToken const *) self;

        printf("BG_CONTROL:%s", self_->super.vptr->getValue(self));
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
static void BGControlToken_take_(Token const * const self,
                                 StringIterator const * const iter)
{
        BGControlToken const * const self_ = (BGControlToken const *) self;

        // copy background command symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c CommentToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c CommentToken object
 */
static void CommentToken_print_(Token const * const self)
{
        CommentToken const * const self_ = (CommentToken const *) self;

        printf("COMMENT:%s", self_->super.vptr->getValue(self));
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
static void CommentToken_take_(Token const * const self,
                               StringIterator const * const iter)
{
        CommentToken const * const self_ = (CommentToken const *) self;

        // copy comment symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c InputRedirToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c InputRedirToken object
 */
static void InputRedirToken_print_(Token const * const self)
{
        InputRedirToken const * const self_ = (InputRedirToken const *) self;

        printf("INPUT_REDIR:%s", self_->super.vptr->getValue(self));
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
static void InputRedirToken_take_(Token const * const self,
                                  StringIterator const * const iter)
{
        InputRedirToken const * const self_ = (InputRedirToken const *) self;

        // copy input redirection symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c NewlineToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c NewlineToken object
 */
static void NewlineToken_print_(Token const * const self)
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
static void NewlineToken_take_(Token const * const self,
                               StringIterator const * const iter)
{
        NewlineToken const * const self_ = (NewlineToken const *) self;

        // copy output redirection symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c OutputRedirToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c OutputRedirToken object
 */
static void OutputRedirToken_print_(Token const * const self)
{
        OutputRedirToken const * const self_ = (OutputRedirToken const *) self;

        printf("OUTPUT_REDIR:%s", self_->super.vptr->getValue(self));
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
static void OutputRedirToken_take_(Token const * const self,
                                   StringIterator const * const iter)
{
        OutputRedirToken const * const self_ = (OutputRedirToken const *) self;

        // copy output redirection symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
        free(slice);
}

/**
 * @brief Implementation of @c WordToken::Token::print().
 *
 * Prints a pretty formatted display of the @c Token object.
 * @param self pointer to @c WordToken object
 */
static void WordToken_print_(Token const * const self)
{
        WordToken const * const self_ = (WordToken const *) self;

        printf("WORD:%s", self_->super.vptr->getValue(self));
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
static void WordToken_take_(Token const * const self,
                            StringIterator const * const iter)
{
        WordToken const * const self_ = (WordToken const *) self;

        // copy word into token
        char *slice = iter->vptr->munchWord(iter);
        self_->super.vptr->setValue(self, slice);
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
void BGControlToken_ctor(BGControlToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .print = &BGControlToken_print_,
                .setValue = &Token_setValue_,
                .take = &BGControlToken_take_,
        };
        Token_ctor(&self->super, (TokenType) TOK_CTRL_BG);
        self->super.vptr = &vtbl;
}

void BGControlToken_dtor(BGControlToken *self)
{
        Token_dtor(&self->super);
}

void CommentToken_ctor(CommentToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .print = &CommentToken_print_,
                .setValue = &Token_setValue_,
                .take = &CommentToken_take_,
        };
        Token_ctor(&self->super, (TokenType) TOK_CMT);
        self->super.vptr = &vtbl;
}

void CommentToken_dtor(CommentToken *self)
{
        Token_dtor(&self->super);
}

void InputRedirToken_ctor(InputRedirToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .print = &InputRedirToken_print_,
                .setValue = &Token_setValue_,
                .take = &InputRedirToken_take_,
        };
        Token_ctor(&self->super, (TokenType) TOK_REDIR_INPUT);
        self->super.vptr = &vtbl;
}

void InputRedirToken_dtor(InputRedirToken *self)
{
        Token_dtor(&self->super);
}

void NewlineToken_ctor(NewlineToken *self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .print = &NewlineToken_print_,
                .setValue = &Token_setValue_,
                .take = &NewlineToken_take_,
        };
        Token_ctor(&self->super, (TokenType) TOK_CTRL_NEWLINE);
        self->super.vptr = &vtbl;
}

void NewlineToken_dtor(NewlineToken *self)
{
        Token_dtor(&self->super);
}

void OutputRedirToken_ctor(OutputRedirToken *self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .print = &OutputRedirToken_print_,
                .setValue = &Token_setValue_,
                .take = &OutputRedirToken_take_,
        };
        Token_ctor(&self->super, (TokenType) TOK_REDIR_OUTPUT);
        self->super.vptr = &vtbl;
}

void OutputRedirToken_dtor(OutputRedirToken *self)
{
        Token_dtor(&self->super);
}

void WordToken_ctor(WordToken *self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .print = &WordToken_print_,
                .setValue = &Token_setValue_,
                .take = &WordToken_take_,
        };
        Token_ctor(&self->super, (TokenType) TOK_WORD);
        self->super.vptr = &vtbl;
}

void WordToken_dtor(WordToken *self)
{
        Token_dtor(&self->super);
}
