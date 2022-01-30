/**
 * @file node.c
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief For storing a token node.
 */
#include <stdio.h>

#include "node.h"

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
 * OBJECTS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/**
 * @brief Hides @s Node members from sub-classes and client code.
 */
struct NodePrivate {
        NodeType type; /**< the node's type */
        NodeValue *value; /**< the node's data */
};

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
 * @brief Pretty-prints info for a bg control node.
 * @param self pointer to node
 */
static void BGControlNode_print_(Node const * const self)
{
        (void) self;
        printf("BG_CONTROL = TRUE,\n");
}

/**
 * @brief Pretty-prints info for a command node
 * @param self pointer to node
 */
static void CommandNode_print_(Node const * const self)
{
        NodeValue *node_value;
        CommandValue value;

        node_value = self->vptr->getValue(self);
        value = node_value->cmd_value;

        printf("CMD (\n");
        printf("\t\tCOMMAND_NAME = %s,\n",
               value.argv[0]->super.vptr->getValue((Token *) value.argv[0]));
        for (size_t i = 1; i < value.argc; i++) {
                printf("\t\tARGUMENT [%zu] = %s,\n",
                       i, value.argv[i]->super.vptr->getValue((Token *) value.argv[i]));
        }
        printf("),\n");
}

/**
 * @brief Pretty-prints info for a comment node
 * @param self pointer to node
 */
static void CommentNode_print_(Node const * const self)
{
        (void) self;
        printf("COMMENT = TRUE,\n");
}

/**
 * @brief Pretty-prints info for an io redir node.
 * @param self pointer to node
 */
static void IORedirNode_print_(Node const * const self)
{
        NodeValue *node_value;
        IORedirValue value;
        ShellTokenType type;

        node_value = self->vptr->getValue(self);
        value = node_value->ioredir_value;
        type = value.type;
        WordToken const * const token = (WordToken const *) value.stream;

        printf("IO_REDIRECTION (\n");
        if (type == TOK_REDIR_INPUT) {
                printf("\t\tSTDIN = %s\n",
                       token->super.vptr->getValue((const Token *const) token));
        } else if (type == TOK_REDIR_OUTPUT) {
                printf("\t\tSTDOUT = %s\n",
                       token->super.vptr->getValue((const Token *const) token));
        } else {
                // error
        }
        printf("),\n");
}

// TODO: look into options for implementing deep copying of data
/**
 * @brief Return the node's type identifier.
 * @param self node
 * @return node type identifier
 */
static inline NodeType Node_getType_(Node const *self)
{
        return self->_private->type;
}

/**
 * @brief Return the node's data.
 * @param self node
 * @return node data
 */
static inline NodeValue *Node_getValue_(Node const *self)
{
        return self->_private->value;
}

/**
 * @brief Update the node's data to @p value.
 * @param self node
 * @param value value to update @p self with
 */
static inline void Node_setValue_(Node const *self, NodeValue *value)
{
        self->_private->value = value;
}

/**
 * @brief Pretty prints a @c Node object.
 * @param self pointer to @c Node object
 */
static void Node_print_(Node const * const self)
{
        NodeType type = self->_private->type;

        switch (type) {
                case NODE_CTRL_BG:
                        BGControlNode_print_(self);
                        break;
                case NODE_CMD:
                        CommandNode_print_(self);
                        break;
                case NODE_CMT:
                        CommentNode_print_(self);
                        break;
                case NODE_REDIR:
                        IORedirNode_print_(self);
                        break;
                default:
                        break;
        }
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
void Node_ctor(Node * const self, NodeType type)
{
        static struct NodeVtbl const vtbl = {
                .getType = &Node_getType_,
                .getValue = &Node_getValue_,
                .print = &Node_print_,
                .setValue = &Node_setValue_,
        };
        self->vptr = &vtbl;
        self->_private = malloc(sizeof(struct NodePrivate));

        self->_private->type = type;
        self->_private->value = NULL;
}

void Node_dtor(Node *self)
{
        self->vptr = NULL;

        self->_private->type = NODE_0;
        self->_private->value = NULL;

        free(self->_private);
        self->_private = NULL;
}
