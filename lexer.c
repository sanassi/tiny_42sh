#include "lexer.h"
#include "hash_map/hash_map.h"
#include <stddef.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

//TODO : rename lexer to lexer

/*tools*/

char *get_substr(char *s, size_t start, size_t end)
{
    if(!s)
        return NULL;
    if (start > end)
        return NULL;
    if (end - start > strlen(s))
        return NULL;
    if (end > strlen(s))
        return NULL;

    size_t sub_size = end - start;
    char *sub = calloc(sub_size + 1, sizeof(char));
    memcpy(sub, s + start, sub_size);

    return sub;
}

bool str_equ(char *s1, char *s2)
{
    return strcmp(s1, s2) == 0;
}

bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

bool is_alpha(char c)
{
    if (c >= 'A' && c <= 'Z')
        return true;
    if (c >= 'a' && c <= 'z')
        return true;
    return false;
}

bool is_alphanum(char c)
{
    return is_alpha(c) || is_digit(c);
}

/*-----*/


bool lexer_is_at_end(struct lexer *l)
{
    if (!l)
        return false;

    return l -> current >= l ->src_len;
}

char lexer_advance(struct lexer *l)
{
    if (!l)
        return '\0';

    char c = l -> src[l -> current];
    l -> current += 1;

    return c;
}

char lexer_peek(struct lexer *l)
{
    if (!l)
        return '\0';

    if (lexer_is_at_end(l))
        return '\0';

    return l -> src[l -> current];
}

bool lexer_match(struct lexer *l, char expected)
{
    if (!l)
        return false;

    if (lexer_is_at_end(l))
        return false;

    if (l->src[l -> current] != expected)
        return false;

    l -> current += 1;

    return true;
}

void token_list_append(struct token_list **t_list, struct token *tok)
{
    if (!tok)
        err(1, "token_list_append : token is NULL");

    if (*t_list == NULL)
    {
        *t_list = calloc(1, sizeof(struct token_list));
        (*t_list)->t = tok;
        return;
    }

    struct token_list *tmp = *t_list;

    while (tmp -> next)
    {
        tmp = tmp -> next;
    }

    tmp -> next = calloc(1, sizeof(struct token_list));
    tmp -> next ->t = tok;
}

void lexer_add_token(struct lexer *l, enum TOK_TYPE type, void *literal)
{
   char *text = get_substr(l ->src, l->start, l->current);
   struct token *t = calloc(1, sizeof(struct token));

   t -> type = type;
   t -> lexeme = text;
   t -> literal = literal;
   t -> pos = l -> start;

   token_list_append(&(l->t_list), t);
}

void lexer_add_token_2(struct lexer *l, enum TOK_TYPE type)
{
    lexer_add_token(l, type, NULL);
}

struct hash_map *init_reserved_words(void)
{
    struct hash_map *keywords = hash_map_init(100);
    bool updated = false;

    hm_insert_int(keywords, "if", IF, &updated);
    hm_insert_int(keywords, "then", THEN, &updated);
    hm_insert_int(keywords, "else", ELSE, &updated);
    hm_insert_int(keywords, "elif", ELIF, &updated);
    hm_insert_int(keywords, "fi", FI, &updated);
    hm_insert_int(keywords, "do", DO, &updated);
    hm_insert_int(keywords, "done", DONE, &updated);
    hm_insert_int(keywords, "case", CASE, &updated);
    hm_insert_int(keywords, "esac", ESAC, &updated);
    hm_insert_int(keywords, "while", WHILE, &updated);
    hm_insert_int(keywords, "until", UNTIL, &updated);
    hm_insert_int(keywords, "for", FOR, &updated);
    hm_insert_int(keywords, "{", LBRACE, &updated);
    hm_insert_int(keywords, "}", RBRACE, &updated);
    hm_insert_int(keywords, "!", BANG, &updated);
    hm_insert_int(keywords, "in", IN, &updated);

    return keywords;
}

void word(struct lexer *l)
{
    while (is_alphanum(lexer_peek(l)))
        lexer_advance(l);

    char *text = get_substr(l -> src, l -> start, l ->current);
    enum TOK_TYPE type = hm_get_int(l->keywords, text);
    if (type < 0)
        type = WORD;
    lexer_add_token_2(l, type);
}

void quote(struct lexer *l, const char quote_char)
{
    while (lexer_peek(l) != quote_char && !lexer_is_at_end(l))
        lexer_advance(l);

    char end = lexer_advance(l);
    if (end != quote_char)
        errx(1, "lexer: unexpected character: %c", end);

    char *text = get_substr(l->src, l->start + 1, l -> current - 1);
    lexer_add_token(l, STRING, text);
    //lexer_add_token_2(l, STRING);
}

bool is_quoting_char(const char c)
{
    return c == '"' || c == '\\' || c == '\'';
}

void blank(struct lexer *l)
{
    while (lexer_peek(l) == ' ')
        lexer_advance(l);
}

void comment(struct lexer *l)
{
    while (lexer_peek(l) != '\n' && !lexer_is_at_end(l))
        lexer_advance(l);
    lexer_match(l, '\n');
}

/*too long*/
void lexer_scan_tokens(struct lexer *l)
{
    char c = lexer_advance(l);
    switch (c) 
    {
    case '}':
        lexer_add_token_2(l, LBRACE);
        break;
    case '{':
        lexer_add_token_2(l, RBRACE);
        break;
    case '!':
        lexer_add_token_2(l, BANG);
        break;
    case '&':
        if (lexer_match(l, '&'))
            lexer_add_token_2(l, AND_IF);
        else
            lexer_add_token_2(l, AND);
        break;
    case '|':
        if (lexer_match(l, '|'))
            lexer_add_token_2(l, OR_IF);
        else
            lexer_add_token_2(l, PIPE);
        break;
    case ';':
        if (lexer_match(l, ';'))
            lexer_add_token_2(l, DSEMI);
        break;
    case '<':
        if (lexer_match(l, '<'))
        {
            if (lexer_match(l, '-'))
                lexer_add_token_2(l, DLESSDASH);
            else
                lexer_add_token_2(l, DLESS);
        }
        if (lexer_match(l, '&'))
            lexer_add_token_2(l, LESSAND);
        break;
    case '>':
        if (lexer_match(l, '>'))
            lexer_add_token_2(l, DGREAT);
        if (lexer_match(l, '&'))
            lexer_add_token_2(l, GREATAND);
        break;
    default:
        if (is_alpha(c))
        {
            word(l);
        }
        else if (is_quoting_char(c))
        {
            quote(l, c);
        }
        else if (c == ' ')
        {
            blank(l);
        }
        else if (c == '#')
        {
            comment(l);
        }
        else 
        {
            errx(1, "lexer : unexpected token");
        }
    }
}

int main(void)
{
    struct lexer *l = calloc(1, sizeof(struct lexer));
    l -> src = "echo truc | nc localhost";
    l -> src_len = strlen(l->src);
    l ->keywords = init_reserved_words();

    while (!lexer_is_at_end(l))
    {
        l -> start = l -> current;
        lexer_scan_tokens(l);
    }

    struct token_list *tmp = l -> t_list;
    while (tmp)
    {
        printf(".%s.\n", tmp -> t ->lexeme);
        tmp = tmp -> next;
    }
    return 0;
}
