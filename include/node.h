/**
 * @file node.h
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief For storing a token node.
 */
#ifndef SMALLSH_NODE_H
#define SMALLSH_NODE_H

#include "token-iterator.h"

struct NodeVtbl;
struct NodePrivate;

/**
 * @brief Node object definition.
 */
typedef struct {
        struct NodeVtbl const *vptr; /**< virtual table */
        struct NodePrivate *_private; /**< private data */
} Node;

/**
 * @brief Defines the base token type (class).
 */
typedef enum {
        NODE_0 = 0, /**< placeholder token type */
        NODE_CTRL_BG = 1, /**< background control node */
        NODE_CMD = 2, /**< command node */
        NODE_CMT = 3, /**< comment node */
        NODE_REDIR = 4, /**< io redirection node */
        NUM_NODES = 5, /**< type count */
} NodeType;

/**
 * @brief Generic definition for bg control node, as it doesn't hold any data
 * internally.
 */
typedef int BGControlValue;

/**
 * @brief Holds a command statement, including its arguments and argument count.
 */
typedef struct {
        size_t argc; /** number of arguments, including command name */
        WordToken **argv; /** array of command and following args */
} CommandValue;

/**
 * @brief Generic definition for comment node, as it doesn't hold any data
 * internally.
 */
typedef int CommentValue;

/**
 * @brief Holds an io redirection , including the type of io redirection
 * and the file to redirect to/from.
 */
typedef struct {
        ShellTokenType type; /**< io redirection type */
        WordToken *filename; /**< file to redirect to/from */
} IORedirValue;

/**
 * @brief Base object for storing node data.
 */
typedef union {
        BGControlValue bgctrl_value; /**< background control data */
        CommandValue cmd_value; /**< command statement data */
        CommentValue cmt_value; /**< comment data */
        IORedirValue ioredir_value; /**< io redirection data */
} NodeValue;

/**
 * @brief VTable for @c Node object.
 */
struct NodeVtbl {
        /**
         * @brief Returns the node's type identifier.
         * @param self pointer @c Node object
         * @return the node's type identifier
         */
        NodeType (*getType) (Node const * const self);

        /**
         * @brief Returns data stored by the node.
         * @param self pointer to @c Node object
         * @return union holding node data
         */
        NodeValue *(*getValue) (Node const * const self);

        /**
         * @brief Prints a pretty-formatted version of @p self.
         * @param self pointer to @c Node object
         */
        void (*print) (Node const * const self);

        /**
         * @brief Updates @p self data to @p value.
         * @param self pointer to @c Node object
         * @param value union holding data to assign to @p self
         */
        void (*setValue) (Node const * const self, NodeValue const * const value);
};

/**
 * @brief Constructs a new @c Node object.
 *
 * <br>
 *
 * This is a base class constructor and should only be called via a child class
 * constructor.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c Node object to initialize
 * @param type @c NodeType to assign to @p self
 */
void Node_ctor(Node *self, NodeType type);

/**
 * @brief Destroys a @c Node object, freeing its members and re-initializing
 * them to @c NULL in the process.
 *
 * <br>
 *
 * This is a base class destructor and should only be called via a child class
 * destructor.
 * @param self pointer to @c Node object
 */
void Node_dtor(Node *self);

/**
 * @brief Implementation of @c Node::getType().
 * @param self pointer to @c Node object
 * @return the node's type identifier
 */
NodeType Node_getType_(Node const *self);

/**
 * @brief Implementation of @c Node::getValue().
 * @param self pointer to @c Node object
 * @return the data stored by @p self
 */
NodeValue *Node_getValue_(Node const *self);

/**
 * @brief Implementation of @c Node::setValue().
 * @param self pointer to @c Node object
 * @param value the data to assign to @p self
 */
void Node_setValue_(Node const *self, const NodeValue *value);

#endif //SMALLSH_NODE_H
