#ifndef PARSER_H
#define PARSER_H

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "hash_map/hash_map.h"

/*
enum TOK_TYPE
{
    T_TILDE,            // ~
    T_SHARP,            // #
    T_DOLLAR,           // $
    T_AMPERS,           // &
    T_STAR,             // *
    T_OPPAR,            // (
    T_CLOPAR,           // )
    T_BSLASH,           //
    T_PIPE,             // |
    T_OCURLBRACK,       // {
    T_CLCURLBRACK,      // }
    T_OBRACK,           // [
    T_CLBRACK,          // ]
    T_SEMICOL,          // ;
    T_QUOTE,            // '
    T_DUBQUOTE,         // "
    T_GT,               // >
    T_LT,               // <
    T_SLASH,            //
    T_QMARK,            // ?
    T_NOT,              // !
    T_DUBGT,           // >>
    T_DUBLT,             // <<
    T_GTAMP, // >&
    T_LTAMP, // <&
    T_GTPIPE, // >|
    T_LTGT, // <>
    T_WORD,
    T_SPACE
}
*/

enum TOK_TYPE
{
    AND_IF, OR_IF, DSEMI,
    DLESS, DGREAT, LESSAND, GREATAND, LESSGREAT, DLESSDASH,
    CLOBBER,
    IF, THEN, ELSE, ELIF, FI, DO, DONE,
    CASE, ESAC, WHILE, UNTIL, FOR,
    LBRACE, RBRACE, BANG,
    LESS, GREAT, SCOLON, AND,
    IN,
    NEWLINE,
};

#define NB_TOKENS 29
char token_str[NB_TOKENS][10] =
{
    "~", "#", "$", "&", "*", "(", ")", "\\", "|", "{", "}", "[", "]",
    ";", "'", "\"", ">", "<", "/\\", "?", "!", ">>", "<<", ">&", "<&",
    ">|", "<>", "word", " "
};

struct token
{
    enum TOK_TYPE type;

    char *lexeme;
    void *literal; // for numbers, strings or ids
    size_t pos;
};

struct token_list
{
    struct token *t;
    struct token_list *next;
};

struct parser
{
    char *src;
    size_t src_len;

    struct token_list *t_list;
    size_t nb_tokens;
    size_t start; // save start pos of current token
    size_t current; // used to advance in the string
};

bool parser_is_at_end(struct parser *p);

char parser_advance(struct parser *p);

char parser_peek(struct parser *p);

void parser_scan_tokens(struct parser *p);

//void parser_add_token(struct parser *p, struct token *t);

#endif
