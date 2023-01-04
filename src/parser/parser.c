#include "parser.h"
#include "lexer/lexer.h"
#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

struct parser *parser_init(char *input)
{
    struct parser *p = calloc(1, sizeof(struct parser));
    p -> current = 0;
    p -> lexer = lexer_init(input);

    /*lex the entire input for now*/

    lexer_lex(p ->lexer);

    return p;
}

bool parser_is_at_end(struct parser *p)
{
    if (!p)
        errx(1, "Error - parser_is_at_end : parser is NULL");

    return token_list_get_at(p -> lexer -> t_list, p -> current)->type == END;
}

struct token *parser_peek(struct parser *p)
{
    if (!p)
        errx(1, "Error - parser_peek : parser is NULL");

    return token_list_get_at(p -> lexer -> t_list, p -> current);
}

struct token *parser_advance(struct parser *p)
{
    if (!p)
        errx(1, "Error - parser_advance : parser is NULL");

    if (parser_is_at_end(p))
        return token_list_get_at(p -> lexer ->t_list, p -> current);

    p -> current += 1;
    //struct token *t = lexer_get_next_token(p -> lexer);

    struct token *t = token_list_get_at(p -> lexer -> t_list, p -> current - 1);
    return t;
}

void parser_free(struct parser *p)
{
    if (!p)
        errx(1, "Error - parser_free : parser is NULL");
    lexer_free(p -> lexer);
    free(p);
}

bool parser_check(struct parser *p, enum TOK_TYPE expected_type)
{
     if (!p)
         errx(1, "Error - parser_check : parser is NULL");

     return parser_peek(p) -> type == expected_type;
}

bool parser_match(struct parser *p, size_t count, ...)
{
    va_list ptr;
    va_start(ptr, count);

    for (size_t i = 0; i < count; i++)
    {
        if (parser_check(p, va_arg(ptr, int)))
        {
            parser_advance(p);
            return true;
        }
    }

    va_end(ptr);

    return false;
}

/*ast stuff*/
struct ast_node_simple_cmd *ast_node_simple_cmd_alloc()
{
    struct ast_node_simple_cmd *node = 
        calloc(1, sizeof(struct ast_node_simple_cmd));

    (node -> base).type = AST_NODE_SIMPLE_CMD;
    node -> cmd_args = calloc(1, sizeof(char *));

    return node;
}

struct ast_node_simple_cmd *
ast_node_simple_cmd_attach(struct ast_node **res)
{
    struct ast_node_simple_cmd *node = ast_node_simple_cmd_alloc();
    *res = &(node -> base);
    return node;
}

/*---------*/
/*recursive descent : only check if input is syntaxically correct*/
void throw_error()
{
    errx(1, "error");
}

void parse_element(struct parser *p, struct ast_node **res)
{
    printf("elt\n");
    if (!parser_match(p, 2, WORD, END))
        throw_error();
}

void parse_simple_command(struct parser *p, struct ast_node **res)
{
    printf("simple\n");
    if (!parser_match(p, 1, WORD))
        throw_error();
   
    printf("here\n");
    do
    {
        parse_element(p, res);
    } while (parser_check(p, WORD));

    if (!parser_match(p, 1, END, NEWLINE))
        throw_error();
}

void parse_command(struct parser *p, struct ast_node **res)
{
    printf("command\n");
    parse_simple_command(p, res);
}

void parse_pipeline(struct parser *p, struct ast_node **res)
{
    printf("pipe\n");
    parse_command(p, res);
}

void parse_and_or(struct parser *p, struct ast_node **res)
{
    printf("andor\n");
    parse_pipeline(p, res);
}

void parse_list(struct parser *p, struct ast_node **res)
{
    printf("list\n");
    parse_and_or(p, res);
}

void parse_input(struct parser *p, struct ast_node **res)
{
    printf("input\n");
    parse_list(p, res);
}

/*----------*/

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;
    struct parser *p = parser_init(argv[1]);

    /*
    while (!parser_is_at_end(p))
    {
        printf("{%s}\n", parser_advance(p)->lexeme);
    }
    */

    struct ast_node *res = NULL;
    parse_input(p, &res);

    parser_free(p);

    return 0;
}
