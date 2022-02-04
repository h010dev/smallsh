/**
 * @file stack.h
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief Stack ADT.
 */
#ifndef SMALLSH_STACK_H
#define SMALLSH_STACK_H

#include <stdbool.h>
#include <stdlib.h>

struct StackPrivate;
typedef struct Stack Stack;

/**
 * @brief Stack class containing available methods and private data.
 */
struct Stack {
        /**
         * @brief Determine if the stack is empty or not.
         * @param self stack
         * @return true if empty, false if not
         */
        bool (*is_empty) (Stack const * const self);

        /**
         * @brief Determine if the stack is full or not.
         * @param self stack
         * @return true if full, false if not
         */
        bool (*is_full) (Stack const * const self);

        /**
         * @brief Get a copy of the item at the top of the stack, without
         * popping it off.
         * @param self stack
         * @return copy of item at the top of the stack
         */
        void *(*peek) (Stack const * const self);

        /**
         * @brief Pop @p item from the top of the stack.
         * @param self stack
         * @return previous item at top of the stack
         */
        void **(*pop) (Stack const * const self);

        /**
         * @brief Push @p item onto the top of the stack.
         * @param self stack
         * @param item item to add to stack
         */
        void (*push) (Stack const * const self, void **item);

        /**
         * @brief Hide internal members.
         */
        struct StackPrivate *private;
};

/**
 * @brief Initialize @p self with a max size equal to @p size.
 * @param self stack
 * @param size stack max size
 */
void stack_ctor(Stack *self, size_t size);

/**
 * @brief Destroy @p self.
 * @param self stack
 */
void stack_dtor(Stack *self);

#endif //SMALLSH_STACK_H
