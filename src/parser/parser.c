/**
 * @file parser.c
 * @author Mohamed Al-Hussein
 * @date 27 Jan 2022
 * @brief For parsing a stream of tokens into nodes for later evaluation.
 */
#include "parser/parser.h"
#include "parser/token-iterator.h"
#include "parser/node.h"
#include "parser/lexer.h"
#include "utils/stack.h"

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
struct ParserPrivate {
        size_t n_tokens;
        Token **tokens;
        Stack *stack;
};

/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/**
 * @brief Returns a new node for the bg control token, and advances the iterator.
 * @param iter iterator containing token stream
 * @return new node holding bg control data
 */
static Node *parse_bgctrl (TokenIterator const *iter)
{
        // make new bg control node
        Node *node = malloc(sizeof(Node));
        Node_ctor(node, NODE_CTRL_BG);

        // consume bg control token
        (void) iter->vptr->next(iter);

        // no need to store the actual symbol; any non-zero value will do
        BGControlValue bg_val = 1;
        NodeValue *val = malloc(sizeof(NodeValue));
        val->bgctrl_value = bg_val;
        node->vptr->setValue((Node *) node, val);

        return node;
}

/**
 * @brief Returns a new node for the command token, and advances the iterator.
 * @param iter iterator containing token stream
 * @retur new node holding command data
 */
static Node *parse_cmd(TokenIterator const *iter)
{
        // make new command node
        Node *node = malloc(sizeof(Node));
        Node_ctor(node, NODE_CMD);

        // make new command value to store command and args
        CommandValue cmd_val;

        // parse args into command value
        size_t buf_size = 1; // buffer size for argv
        cmd_val.argc = 0;
        cmd_val.argv = malloc(buf_size * sizeof(WordToken));

        while (iter->vptr->has_next(iter)) {
                Token tok;
                WordToken **tmp;

                // check if any words left to take
                tok = iter->vptr->peek(iter, 0);
                if (!is_tok_word(tok)) {
                        break; // done
                }

                // resize buf if full
                if (cmd_val.argc >= buf_size) {
                        buf_size *= 2;
                        tmp = realloc(cmd_val.argv, buf_size * sizeof(WordToken));
                        if (tmp == NULL) {
                                return NULL; // error
                        }
                        cmd_val.argv = tmp;
                }

                // take word
                cmd_val.argv[cmd_val.argc++] = (WordToken *) iter->vptr->next(iter);
        }
        NodeValue *val = malloc(sizeof(NodeValue));
        val->cmd_value = cmd_val;

        // add parsed words to command node
        node->vptr->setValue((Node *) node, val);

        return node;
}

/**
 * @brief Returns a new node for the comment token, and advances the iterator.
 * @param iter iterator containing token stream
 * @return new node holding comment data
 */
static Node *parse_cmt(TokenIterator const *iter)
{
        // make new comment node
        Node *node = malloc(sizeof(Node));
        Node_ctor(node, NODE_CMT);

        // consume comment token
        (void) iter->vptr->next(iter);

        // no need to store the actual symbol; any non-zero value will do
        CommentValue cmt_val = 1;
        NodeValue *val = malloc(sizeof(NodeValue));
        val->cmt_value = cmt_val;
        node->vptr->setValue((Node *) node, val);

        return node;
}

/**
 * @brief Returns a new node for an io redirection token, and advances the
 * iterator.
 * @param iter iterator containing token stream
 * @return new node holding io redirection data
 */
static Node *parse_ioredir(TokenIterator const *iter)
{
        // make new io redirection node
        Node *node = malloc(sizeof(Node));
        Node_ctor(node, NODE_REDIR);

        // filename should be a word token
        Token tok = iter->vptr->peek(iter, 1);
        if (!is_tok_word(tok)) {
                return NULL; // error
        }

        // make new io redirection value to store io data in
        IORedirValue io_val;

        // set flag depending on if op is an input or output redirection op
        tok = iter->vptr->peek(iter, 0);
        if (is_tok_redir_input(tok)) {
                io_val.type = TOK_REDIR_INPUT;
        } else if (is_tok_redir_output(tok)) {
                io_val.type = TOK_REDIR_OUTPUT;
        } else {
                return NULL; // error
        }

        // consume redirection op token and filename word token
        (void) iter->vptr->next(iter);
        io_val.stream = (WordToken *) iter->vptr->next(iter);
        NodeValue *val = malloc(sizeof(NodeValue));
        val->ioredir_value = io_val;

        // add parsed io data to node
        node->vptr->setValue((Node *) node, val);

        return node;
}

static size_t Parser_parse_(Parser * const self, char *buf)
{
        // parse stream into tokens
        self->_private->tokens = malloc(MAX_TOKENS * sizeof(Token));
        self->_private->n_tokens = generate_tokens(buf, MAX_TOKENS,
                                                   self->_private->tokens);

        // parse tokens into nodes
        self->_private->stack = malloc(sizeof(Stack));
        Stack_ctor(self->_private->stack, MAX_TOKENS);
        size_t n_node = parse(self->_private->n_tokens, self->_private->tokens,
                              self->_private->stack);

        return n_node;
}

static void Parser_cleanup_(Parser * const self, bool print_tree)
{
        while (!self->_private->stack->isEmpty(self->_private->stack)) {
                Node *node = (Node *) self->_private->stack->pop(self->_private->stack);

                if (print_tree)
                        node->vptr->print(node);

                // cleanup
                NodeValue *value = node->vptr->getValue(node);
                NodeType type = node->vptr->getType(node);
                if (type == NODE_CMD) {
                        CommandValue *cmd;
                        cmd = &value->cmd_value;
                        for (size_t i = 0; i < cmd->argc; i++) {
                                cmd->argv[i] = NULL;
                        }
                        free(cmd->argv);
                        cmd->argv = NULL;
                }
                Node_dtor(node);
                free(node);
                node = NULL;
        }

        // cleanup
        for (size_t i = 0; i < self->_private->n_tokens; i++) {
                Token_dtor(self->_private->tokens[i]);
                free(self->_private->tokens[i]);
        }
        free(self->_private->tokens);
        Stack_dtor(self->_private->stack);
        free(self->_private->stack);
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
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
size_t parse(size_t n_tokens, Token *tokens[n_tokens], Stack *node_stack)
{
        // initialize iterator with token stream
        TokenIterator iter;
        TokenIterator_ctor(&iter, n_tokens, tokens);

        // evaluate tokens from iterator stream
        size_t count = 0; // number of nodes consumed
        while (iter.vptr->has_next(&iter) && !node_stack->isFull(node_stack)) {
                Token tok = iter.vptr->peek(&iter, 0);

                if (is_tok_cmt(tok)) {
                        node_stack->push(node_stack, (void **) parse_cmt(&iter));
                        count++;
                        break; // done
                } else if (is_tok_ctrl_bg(tok)) {
                        node_stack->push(node_stack, (void **) parse_bgctrl(&iter));
                } else if (is_tok_redir(tok)) {
                        node_stack->push(node_stack, (void **) parse_ioredir(&iter));
                } else if (is_tok_word(tok)) {
                        node_stack->push(node_stack, (void **) parse_cmd(&iter));
                } else {
                        break; // error
                }

                count++;
        }

        TokenIterator_dtor(&iter);

        return count;
}

void Parser_ctor(Parser *self)
{
        self->parse = &Parser_parse_;
        self->cleanup = &Parser_cleanup_;
        self->_private = malloc(sizeof(struct ParserPrivate));
}

void Parser_dtor(Parser *self)
{
        self->parse = NULL;
        self->cleanup = NULL;
        free(self->_private);
}
