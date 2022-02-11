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
        size_t cmd_argc; /**< argument count */
        char **cmd_argv; /**< argument list */
} StmtCmd;

/**
 * @brief STDIN Stream object.
 *
 * A stdin stream is composed of a list of filenames, and a count of the
 * number of filenames. These streams will be later opened for stdin.
 */
typedef struct {
        size_t stdin_num_streams; /**< number of file streams */
        char **stdin_streams; /**< stream list */
} StmtStdin;

/**
 * @brief STDOUT Stream object.
 *
 * A stdout stream is composed of a list of filenames, and a count of the
 * number of filenames. These streams will be later opened for stdout.
 */
typedef struct {
        size_t stdout_num_streams; /**< number of file streams */
        char **stdout_streams; /**< stream list */
} StmtStdout;

/**
 * @brief Statement flags hold additional information on statement command
 * boolean properties.
 */
typedef enum {
        FLAGS_NONE = 0, /**< default flag */
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
        StmtCmd *stmt_cmd; /**< command */
        StmtStdin *stmt_stdin; /**< stdin file streams */
        StmtStdout *stmt_stdout; /**< stdout file streams */
        StmtFlags stmt_flags; /**< special properties */
} Statement;

/**
 * @brief IO Redirection types are used for determining which default
 * IO stream to operate on.
 */
typedef enum {
        IOREDIR_STDIN = 0, /**< stdin stream */
        IOREDIR_STDOUT = 1, /**< stdout stream */
} IORedirType;

/**
 * @brief Create and initialize a new @c Statement object.
 * @return new @c Statement object
 * @note Caller is responsible for freeing structure via @c statement_del.
 */
Statement *statement_new(void);

/**
 * @brief Deletes @p stmt and frees its associated data.
 * @param stmt @c Statement object to delete
 */
void statement_del(Statement **stmt);

/**
 * @brief Pretty-prints a @c Statement object to stdout.
 * @param self @c Statement object to print
 */
void statement_print(Statement const *self);

#endif //SMALLSH_STATEMENT_H
