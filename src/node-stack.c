/**
 * @file node-stack.c
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief Implementation of stack ADT to be used for storing parsed statement
 * nodes.
 */
#include "node-stack.h"
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
 * @brief Internal data representation of node stack.
 */
struct StackPrivate {
        size_t size; /**< maximum size of stack */
        size_t top; /**< index of top of stack */
        Node **items; /**< stack items as node array */
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
 * @brief Implementation of @c Stack::NodeStack::isEmpty().
 * @param self stack
 * @return true if the stack is empty, false otherwise
 */
static inline bool NodeStack_isEmpty_(Stack const * const self)
{
        return self->_private->top == 0;
}

/**
 * @brief Implementation of @c Stack::NodeStack::isFull().
 * @param self stack
 * @return true if the stack is full, false otherwise
 */
static inline bool NodeStack_isFull_(Stack const * const self)
{
        return self->_private->top == self->_private->size;
}

/**
 * @brief Implementation of @c Stack::NodeStack::peek().
 * @param self stack
 * @return item at top of stack, or @c NULL if stack is empty
 * @note Caller should validate if stack is empty before calling this
 * method, or instead check the return value for @c NULL.
 */
static inline void *NodeStack_peek_(Stack const * const self)
{
        if (self->isEmpty(self)) {
                // error
                return NULL;
        }

        return (void *) self->_private->items[self->_private->top - 1];
}

/**
 * @brief Implementation of @c Stack::NodeStack::pop().
 * @param self stack
 * @return item from top of stack, or @c NULL if stack is empty
 * @note Caller should validate if stack is empty before calling this
 * method, or instead check the return value for @c NULL.
 */
static inline void **NodeStack_pop_(Stack const * const self)
{
        if (self->isEmpty(self)) {
                // error
                return NULL;
        }

        return (void *) self->_private->items[--self->_private->top];
}

/**
 * @brief Implementation of @c Stack::NodeStack::push().
 * @pre It is assumed that the stack is not full before calling this method.
 * Caller is responsible for checking if this is true by calling
 * @c NodeStack::isFull() before pushing a new item to the stack. Behavior is
 * undefined otherwise.
 * @param self stack
 * @param item item to push to stack
 */
static inline void NodeStack_push_(Stack const * const self, void **item)
{
        self->_private->items[self->_private->top++] = (Node *) item;
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
void NodeStack_ctor(Stack *self, size_t size)
{
        // initialize stack methods
        self->isEmpty = &NodeStack_isEmpty_;
        self->isFull = &NodeStack_isFull_;
        self->peek = &NodeStack_peek_;
        self->pop = &NodeStack_pop_;
        self->push = &NodeStack_push_;

        // initialize stack data
        Node **stack_ = malloc(size * sizeof(Node *));
        self->_private = malloc(sizeof(struct StackPrivate));
        self->_private->items = stack_;
        self->_private->size = size;
        self->_private->top = 0;
}

// TODO: see if its necessary to free individual stack items or not
void NodeStack_dtor(Stack *self)
{
        // un-initialize stack methods
        self->isEmpty = NULL;
        self->isFull = NULL;
        self->peek = NULL;
        self->pop = NULL;
        self->push = NULL;

        // un-initialize stack data
        self->_private->size = 0;
        self->_private->top = 0;
        free(self->_private->items);
        self->_private->items = NULL;
        free(self->_private);
        self->_private = NULL;
}
