/**
 * @file node-stack.h
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief Implementation of stack ADT to be used for storing parsed statement
 * nodes.
 */
#ifndef SMALLSH_NODE_STACK_H
#define SMALLSH_NODE_STACK_H

#include "stack.h"

/**
 * @brief Create a new stack with a max size of @p size.
 * @param self stack object to initialize
 * @param size maximum size of stack
 */
void NodeStack_ctor(Stack *self, size_t size);

/**
 * @brief Destroys @c self and re-initializes values to @c NULL.
 * @param self stack to destroy
 * @Note Caller is responsible for freeing @p self afterwards if necessary.
 */
void NodeStack_dtor(Stack *self);

#endif //SMALLSH_NODE_STACK_H
