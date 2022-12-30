#ifndef LEXER_H
#define LEXER_H

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
    /*operators*/
    AND_IF, OR_IF, DSEMI,
    DLESS, DGREAT, LESSAND, GREATAND, LESSGREAT, DLESSDASH,
    CLOBBER,

    /*reserved words*/
    IF, THEN, ELSE, ELIF, FI, DO, DONE,
    CASE, ESAC, WHILE, UNTIL, FOR,
    LBRACE, RBRACE, BANG,
    IN,

    /*misc*/
    LESS, GREAT, SCOLON,
    NEWLINE,
    WORD,
    STRING,

    PIPE, AND,

    NAME, ASSIGNMENT_WORD
};

#define NB_TOKENS 36
char token_str[NB_TOKENS][20] =
{
    "&&", "||", ";;", "<<", ">>", "<&",
    ">&", "<>", "<<-", "|>",
    "if", "then", "else", "elif", "fi", "do", "done",
    "case", "esac", "while", "until", "for",
    "{", "}", "!", "in", "<", ">", ";", "\n",
    "WORD", "STRING", "|", "&", "NAME", "ASSIGNMENT_WORD",
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

struct lexer
{
    char *src;
    size_t src_len;

    struct token_list *t_list;
    size_t nb_tokens;
    size_t start; // save start pos of current token
    size_t current; // used to advance in the string

    struct hash_map *keywords;

    bool quoting;
};

bool lexer_is_at_end(struct lexer *p);

char lexer_advance(struct lexer *p);

char lexer_peek(struct lexer *p);

void lexer_scan_tokens(struct lexer *p);

void free_token(struct token *t);

void free_token_list(struct token_list *t_list);

void lexer_free(struct lexer *l);

//void lexer_add_token(struct lexer *p, struct token *t);

#endif
