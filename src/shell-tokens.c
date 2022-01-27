/**
 * @file shell-tokens.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief For parsing and storing the different types of tokens of the shell.
 */
#include "shell-tokens.h"

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
 * @brief Implementation of @c ArgumentToken::Token::take().
 *
 * Iterates over a single word and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed an argument token.
 * @param self pointer to @c ArgumentToken object
 * @param iter pointer to @c StringIterator object
 */
static void ArgumentToken_take_(Token const * const self,
                                StringIterator const * const iter)
{
        ArgumentToken const * const self_ = (ArgumentToken const *) self;

        // copy word into token
        char *slice = iter->vptr->munchWord(iter);
        self_->super.vptr->setValue(self, slice);
}

/**
 * @brief Implementation of @c BackgroundCommandToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed a background command token.
 * @param self pointer to @c BackgroundCommandToken object
 * @param iter pointer to @c StringIterator object
 */
static void BackgroundCommandToken_take_(Token const * const self,
                                         StringIterator const * const iter)
{
        BackgroundCommandToken const * const self_ =
                (BackgroundCommandToken const *) self;

        // copy background command symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
}

/**
 * @brief Implementation of @c CommandToken::Token::take().
 *
 * Iterates over a single word and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed a command token.
 * @param self pointer to @c CommandToken object
 * @param iter pointer to @c StringIterator object
 */
static void CommandToken_take_(Token const * const self,
                               StringIterator const * const iter)
{
        CommandToken const * const self_ = (CommandToken const *) self;

        // copy word into token
        char *slice = iter->vptr->munchWord(iter);
        self_->super.vptr->setValue(self, slice);
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
}

/**
 * @brief Implementation of @c InputRedirectionToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed an input redirection token.
 * @param self pointer to @c InputRedirectionToken object
 * @param iter pointer to @c StringIterator object
 */
static void InputRedirectionToken_take_(Token const * const self,
                                        StringIterator const * const iter)
{
        InputRedirectionToken const * const self_ =
                (InputRedirectionToken const *) self;

        // copy input redirection symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
}

/**
 * @brief Implementation of @c OutputRedirectionToken::Token::take().
 *
 * Iterates over a single character and then stores the copy.
 * @pre It is the caller's responsibility to ensure that the data being
 * copied over is indeed an output redirection token.
 * @param self pointer to @c OutputRedirectionToken object
 * @param iter pointer to @c StringIterator object
 */
static void OutputRedirectionToken_take_(Token const * const self,
                                         StringIterator const * const iter)
{
        OutputRedirectionToken const * const self_ =
                (OutputRedirectionToken const *) self;

        // copy output redirection symbol into token
        char *slice = iter->vptr->munchChar(iter);
        self_->super.vptr->setValue(self, slice);
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
void ArgumentToken_ctor(ArgumentToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .setValue = &Token_setValue_,
                .take = &ArgumentToken_take_,
        };
        Token_ctor(&self->super, ARGUMENT_TOKEN);
        self->super.vptr = &vtbl;
}

void ArgumentToken_dtor(ArgumentToken *self)
{
        Token_dtor(&self->super);
}

void BackgroundCommandToken_ctor(BackgroundCommandToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .setValue = &Token_setValue_,
                .take = &BackgroundCommandToken_take_,
        };
        Token_ctor(&self->super, BACKGROUND_COMMAND_TOKEN);
        self->super.vptr = &vtbl;
}

void BackgroundCommandToken_dtor(BackgroundCommandToken *self)
{
        Token_dtor(&self->super);
}

void CommandToken_ctor(CommandToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .setValue = &Token_setValue_,
                .take = &CommandToken_take_,
        };
        Token_ctor(&self->super, COMMAND_TOKEN);
        self->super.vptr = &vtbl;
}

void CommandToken_dtor(CommandToken *self)
{
        Token_dtor(&self->super);
}

void CommentToken_ctor(CommentToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .setValue = &Token_setValue_,
                .take = &CommentToken_take_,
        };
        Token_ctor(&self->super, COMMENT_TOKEN);
        self->super.vptr = &vtbl;
}

void CommentToken_dtor(CommentToken *self)
{
        Token_dtor(&self->super);
}

void InputRedirectionToken_ctor(InputRedirectionToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .setValue = &Token_setValue_,
                .take = &InputRedirectionToken_take_,
        };
        Token_ctor(&self->super, INPUT_REDIRECTION_TOKEN);
        self->super.vptr = &vtbl;
}

void InputRedirectionToken_dtor(InputRedirectionToken *self)
{
        Token_dtor(&self->super);
}

void OutputRedirectionToken_ctor(OutputRedirectionToken * const self)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .setValue = &Token_setValue_,
                .take = &OutputRedirectionToken_take_,
        };
        Token_ctor(&self->super, OUTPUT_REDIRECTION_TOKEN);
        self->super.vptr = &vtbl;
}

void OutputRedirectionToken_dtor(OutputRedirectionToken *self)
{
        Token_dtor(&self->super);
}
