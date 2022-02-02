/**
 * @file statement.h
 * @author Mohamed Al-Hussein
 * @date 02 Feb 2022
 * @brief For creating command statements to later be executed.
 */
#ifndef SMALLSH_STATEMENT_H
#define SMALLSH_STATEMENT_H

/**
 * @brief Command object.
 *
 * A command struct is composed of a list of arguments, where the first
 * argument is the command name; and an argument count.
 */
typedef struct {
        size_t argc; /**< argument count */
        char **argv; /**< argument list */
} StmtCmd;

/**
 * @brief STDIN Stream object.
 *
 * A stdin stream is composed of a list of filenames, and a count of the
 * number of filenames. These streams will be later opened for stdin.
 */
typedef struct {
        size_t n; /**< number of file streams */
        char **streams; /**< stream list */
} StmtStdin;

/**
 * @brief STDOUT Stream object.
 *
 * A stdout stream is composed of a list of filenames, and a count of the
 * number of filenames. These streams will be later opened for stdout.
 */
typedef struct {
        size_t n; /**< number of file streams */
        char **streams; /**< stream list */
} StmtStdout;

/**
 * @brief Statement flags hold additional information on statement command
 * boolean properties.
 */
typedef enum {
        FLAGS_0 = 0, /**< default flag */
        FLAGS_BGCTRL = 1, /**< background control flag */
        FLAGS_BUILTIN = 2, /**< builtin command flag */
} StmtFlags;

/**
 * @brief A statement object is composed of the sub-statement objects required
 * to execute a command.
 *
 * This object is later passed on to the execution step of the shell.
 */
typedef struct {
        StmtCmd *cmd; /**< command */
        StmtStdin *stdin_; /**< stdin file streams */
        StmtStdout *stdout_; /**< stdout file streams */
        // StmtStderr *stderr; /* not supported */
        StmtFlags flags; /**< special properties */
} Statement;

/**
 * @brief IO Redirection types are used for determining which default
 * IO stream to operate on.
 */
typedef enum {
        IO_REDIR_STDIN = 0, /**< stdin stream */
        IO_REDIR_STDOUT = 1, /**< stdout stream */
        // IO_REDIR_STDERR = 2, /* not supported */
} IORedirType;

/**
 * @brief Create and initialize a new @c Statement object.
 * @return new @c Statement object
 * @note Caller is responsible for freeing structure via @c Statement_del.
 */
Statement *Statement_new(void);

/**
 * @brief Deletes @p stmt and frees its associated data.
 * @param stmt @c Statement object to delete
 */
void Statement_del(Statement **stmt);

/**
 * @brief Pretty-prints a @c Statement object to stdout.
 * @param self @c Statement object to print
 */
void Statement_print(Statement const *self);

#endif //SMALLSH_STATEMENT_H
