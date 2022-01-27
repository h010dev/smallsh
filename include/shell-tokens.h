/**
 * @file shell-tokens.h
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief Definitions for the different types of tokens parsed by the shell.
 */
#ifndef SMALLSH_SHELL_TOKENS_H
#define SMALLSH_SHELL_TOKENS_H

#include "token.h"

/* *****************************************************************************
 * ArgumentToken
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
/**
 * @brief Defines a token object that parses and stores an argument literal.
 */
typedef struct {
        Token super; /**< inherits from @c Token */
} ArgumentToken;

/**
 * @brief Constructs a new @c ArgumentToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c ArgumentToken object to initialize
 */
void ArgumentToken_ctor(ArgumentToken *self);

/**
 * @brief Destroys an @c ArgumentToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c ArgumentToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void ArgumentToken_dtor(ArgumentToken *self);

/* *****************************************************************************
 * BackgroundCommandToken
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
/**
 * @brief Defines a token object that parses and stores background command
 * symbol.
 */
typedef struct {
        Token super; /**< inherits from @c Token */
} BackgroundCommandToken;

/**
 * @brief Constructs a new @c BackgroundCommandToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c BackgroundCommandToken object to initialize
 */
void BackgroundCommandToken_ctor(BackgroundCommandToken *self);

/**
 * @brief Destroys a @c BackgroundCommandToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c BackgroundCommandToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void BackgroundCommandToken_dtor(BackgroundCommandToken *self);

/* *****************************************************************************
 * CommandToken
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
/**
 * @brief Defines a token object that parses and stores a command literal.
 */
typedef struct {
        Token super; /**< inherits from @c Token */
} CommandToken;

/**
 * @brief Constructs a new @c CommandToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c CommandToken object to initialize
 */
void CommandToken_ctor(CommandToken *self);

/**
 * @brief Destroys a @c CommandToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c CommandToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void CommandToken_dtor(CommandToken *self);

/* *****************************************************************************
 * CommentToken
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
/**
 * @brief Defines a token object that parses and stores a comment symbol.
 */
typedef struct {
        Token super; /**< inherits from @c Token */
} CommentToken;

/**
 * @brief Constructs a new @c CommentToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c CommentToken object to initialize
 */
void CommentToken_ctor(CommentToken *self);

/**
 * @brief Destroys a @c CommentToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c CommentToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void CommentToken_dtor(CommentToken *self);

/* *****************************************************************************
 * InputRedirectionToken
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
/**
 * @brief Defines a token object that parses and stores an input redirection
 * symbol.
 */
typedef struct {
        Token super; /**< inherits from @c Token */
} InputRedirectionToken;

/**
 * @brief Constructs a new @c InputRedirectionToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c InputRedirectionToken object to initialize
 */
void InputRedirectionToken_ctor(InputRedirectionToken *self);

/**
 * @brief Destroys an @c InputRedirectionToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c InputRedirectionToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void InputRedirectionToken_dtor(InputRedirectionToken *self);

/* *****************************************************************************
 * OutputRedirectionToken
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
/**
 * @brief Defines a token object that parses and stores an output redirection
 * symbol.
 */
typedef struct {
        Token super; /**< inherits from @c Token */
} OutputRedirectionToken;

/**
 * @brief Constructs a new @c OutputRedirectionToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c OutputRedirectionToken object to initialize
 */
void OutputRedirectionToken_ctor(OutputRedirectionToken *self);

/**
 * @brief Destroys an @c OutputRedirectionToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c OutputRedirectionToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void OutputRedirectionToken_dtor(OutputRedirectionToken *self);

#endif //SMALLSH_SHELL_TOKENS_H
