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
        size_t count; /**< argument count */
        char **args; /**< argument list */
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
        StmtCmd *cmd; /**< command */
        StmtStdin *infile; /**< stdin file streams */
        StmtStdout *outfile; /**< stdout file streams */
        StmtFlags flags; /**< special properties */
} SH_Statement;

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
 * @note Caller is responsible for freeing structure via @c SH_DestroyStatement.
 */
SH_Statement *SH_CreateStatement(void);

/**
 * @brief Deletes @p stmt and frees its associated data.
 * @param stmt @c Statement object to delete
 */
void SH_DestroyStatement(SH_Statement **stmt);

/**
 * @brief Pretty-prints a @c Statement object to stdout.
 * @param stmt @c Statement object to print
 */
void SH_PrintStatement(SH_Statement const *stmt);

#endif //SMALLSH_STATEMENT_H
