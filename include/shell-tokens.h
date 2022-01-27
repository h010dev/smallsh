/**
 * @file shell-tokens.h
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief Definitions for the different types of tokens parsed by the shell.
 */
#ifndef SMALLSH_SHELL_TOKENS_H
#define SMALLSH_SHELL_TOKENS_H

#include "token.h"

/**
 * @brief Defines the types of tokens our scanner will produce.
 */
typedef enum {
        BG_CONTROL_TOKEN = 1, /**< a background is identified by '&' at the end of the stream */
        COMMENT_TOKEN = 2, /**< a comment is identified by '#' at the start of the stream */
        INPUT_REDIR_TOKEN = 3, /**< an input redirection is a '<' to redirect file io */
        NEWLINE_TOKEN = 4, /**< a newline is identified by '\\n' */
        OUTPUT_REDIR_TOKEN = 5, /**< an output redirection is a '>' to redirect file io */
        WORD_TOKEN = 6, /**< a command is any word at the start of the stream */
        NUM_TOKENS = 7, /**< count of tokens to allow iterating over them */
} ShellTokenType;

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
} BGControlToken;

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
void BGControlToken_ctor(BGControlToken *self);

/**
 * @brief Destroys a @c BackgroundCommandToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c BackgroundCommandToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void BGControlToken_dtor(BGControlToken *self);

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
 * InputRedirToken
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
} InputRedirToken;

/**
 * @brief Constructs a new @c InputRedirToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c InputRedirectionToken object to initialize
 */
void InputRedirToken_ctor(InputRedirToken *self);

/**
 * @brief Destroys an @c InputRedirToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c InputRedirToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void InputRedirToken_dtor(InputRedirToken *self);

/* *****************************************************************************
 * NewlineToken
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
 * @brief Defines a token object that parses and stores a newline character.
 */
typedef struct {
        Token super; /**< inherits from @c Token */
} NewlineToken;

/**
 * @brief Constructs a new @c NewlineToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c NewlineToken object to initialize
 */
void NewlineToken_ctor(NewlineToken *self);

/**
 * @brief Destroys an @c NewlineToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c NewlineToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void NewlineToken_dtor(NewlineToken *self);

/* *****************************************************************************
 * OutputRedirToken
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
} OutputRedirToken;

/**
 * @brief Constructs a new @c OutputRedirToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c OutputRedirToken object to initialize
 */
void OutputRedirToken_ctor(OutputRedirToken *self);

/**
 * @brief Destroys an @c OutputRedirToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c OutputRedirToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void OutputRedirToken_dtor(OutputRedirToken *self);

/* *****************************************************************************
 * WordToken
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
} WordToken;

/**
 * @brief Constructs a new @c WordToken object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c WordToken object to initialize
 */
void WordToken_ctor(WordToken *self);

/**
 * @brief Destroys a @c CommandToken object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self @c CommandToken object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void WordToken_dtor(WordToken *self);

#endif //SMALLSH_SHELL_TOKENS_H
