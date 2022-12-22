#include "parser.h"
#include "hash_map/hash_map.h"
#include <stddef.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

/*-----*/


bool parser_is_at_end(struct parser *p)
{
    if (!p)
        return false;

    return p -> current >= p ->src_len;
}

char parser_advance(struct parser *p)
{
    if (!p)
        return '\0';

    char c = p -> src[p -> current];
    p -> current += 1;

    return c;
}

char parser_peek(struct parser *p)
{
    if (!p)
        return '\0';

    if (parser_is_at_end(p))
        return '\0';

    return p -> src[p -> current];
}

bool parser_match(struct parser *p, char expected)
{
    if (!p)
        return false;

    if (parser_is_at_end(p))
        return false;

    if (p->src[p -> current] != expected)
        return false;

    p -> current += 1;

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

void parser_add_token(struct parser *p, enum TOK_TYPE type, void *literal)
{
   char *text = get_substr(p ->src, p->start, p->current);
   struct token *t = calloc(1, sizeof(struct token));

   t -> type = type;
   t -> lexeme = text;
   t -> literal = literal;
   t -> pos = p -> start;

   token_list_append(&(p->t_list), t);
}

void parser_add_token_2(struct parser *p, enum TOK_TYPE type)
{
    parser_add_token(p, type, NULL);
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

void parser_scan_tokens(struct parser *p)
{
    char c = parser_advance(p);

    switch (c) {
        case '(':
            parser_add_token_2(p, 1);
            break;
        case ')':
            parser_add_token_2(p, 2);
            break;
        case '{':
            parser_add_token_2(p, 3);
            break;
        case '}':
            parser_add_token_2(p, 4);
            break;
        case ' ':
            parser_add_token_2(p, 5);
            break;
    }
}

int main(void)
{
    /*
    struct parser *p = calloc(1, sizeof(struct parser));
    p -> src = "( ( { { } } ) )";
    p -> src_len = strlen(p->src);

    while (!parser_is_at_end(p))
    {
        parser_scan_tokens(p);
    }

    struct token_list *tmp = p -> t_list;
    while (tmp)
    {
        printf("%s\n", token_str[tmp -> t ->type]);
        tmp = tmp -> next;
    }

    printf("\n");
    */

    struct hash_map *words = init_reserved_words();
    hash_map_dump(words);
    return 0;
}
