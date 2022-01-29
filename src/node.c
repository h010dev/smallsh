/**
 * @file node.c
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief For storing a token node.
 */
#include <assert.h>

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
 * @brief Virtual method definition for pretty printing an @c Node object.
 * @param self pointer to @c Node object
 */
static void Node_print_(Node const * const self)
{
        (void) self;
        assert(0);
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
// TODO: look into options for implementing deep copying of data
inline NodeType Node_getType_(Node const *self)
{
        return self->_private->type;
}

inline NodeValue *Node_getValue_(Node const *self)
{
        return self->_private->value;
}

inline void Node_setValue_(Node const *self, const NodeValue *value)
{
        self->_private->value = (NodeValue *) value;
}
