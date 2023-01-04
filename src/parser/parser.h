#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <err.h>

#include "lexer/lexer.h"

enum parser_status
{
    PARSER_DONE,
    PARSER_NO_MATCH,
    PARSER_ERROR,
};

struct parser
{
    struct lexer *lexer;
    size_t current;

    enum parser_status status;
};

enum ast_node_type
{
    AST_NODE_IF,
    AST_NODE_SIMPLE_CMD,
};

struct ast_node
{
    enum ast_node_type type;

    struct ast_node **children;
};

struct ast_node_simple_cmd
{
    struct ast_node base;

    char *cmd_name;
    char **cmd_args;
};

#endif
