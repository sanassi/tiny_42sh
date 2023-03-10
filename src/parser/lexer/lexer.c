#include "lexer.h"
#include "hash_map/hash_map.h"
#include <stddef.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

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
    if (c == '-')
        return true;
    return false;
}

bool is_alphanum(char c)
{
    return is_alpha(c) || is_digit(c);
}

/*-----*/

/*free functions*/

void free_token(struct token *t)
{
    if (!t)
        return;
    free(t -> lexeme);
    free(t -> literal);
    free(t);
}

void free_token_list(struct token_list *t_list)
{
    if (t_list)
    {
        free_token_list(t_list -> next);
        free_token(t_list -> t);
        free(t_list);
    }
}

void lexer_free(struct lexer *l)
{
    if (!l)
        return;

    free_token_list(l->t_list);
    hash_map_free(l -> keywords);
    free(l);
}
/*--------------*/

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

/*token list functions*/
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

struct token *token_list_get_at(struct token_list *t_list, size_t index)
{
    if (!t_list)
        errx(1, "Error - token_list_get_at : list is NULL");

    struct token_list *tmp = t_list;

    while (tmp && index > 0)
    {
        index -= 1;
        tmp = tmp -> next;
    }

    if (!tmp)
        errx(1, "Error - token_list_get_at : index out of range");

    return tmp->t;
}

/*------------------*/
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

struct token *get_last_token(struct token_list *t_list)
{
    if (!t_list)
        return NULL;

    struct token_list *end = t_list;

    while (end -> next)
        end = end -> next;
    return end -> t;
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
    int type = hm_get_int(l->keywords, text); // do not use enum TOK_TYPE
    if (type < 0)
        type = WORD;
    lexer_add_token_2(l, type);
    free(text);
}

void quote(struct lexer *l, const char quote_char)
{
    while (lexer_peek(l) != quote_char && !lexer_is_at_end(l))
        lexer_advance(l);

    char end = lexer_advance(l);
    if (end != quote_char)
        errx(1, "Error - lexer: unexpected character: %c", end);

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
    case '(':
        lexer_add_token_2(l, LPAR);
        break;
    case ')':
        lexer_add_token_2(l, RPAR);
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
        else
            lexer_add_token_2(l, SCOLON);
        break;
    case '<':
        if (lexer_match(l, '<'))
        {
            if (lexer_match(l, '-'))
                lexer_add_token_2(l, DLESSDASH);
            else
                lexer_add_token_2(l, DLESS);
            break;
        }
        else if (lexer_match(l, '>'))
            lexer_add_token_2(l, LESSGREAT);
        else if (lexer_match(l, '&'))
            lexer_add_token_2(l, LESSAND);
        break;
    case '>':
        if (lexer_match(l, '>'))
            lexer_add_token_2(l, DGREAT);
        if (lexer_match(l, '&'))
            lexer_add_token_2(l, GREATAND);
        break;
    case '\n':
        lexer_add_token_2(l, NEWLINE);
        break;
    default:
        if (is_alpha(c))
            word(l);
        else if (is_quoting_char(c))
            quote(l, c);
        else if (c == ' ')
            blank(l);
        else if (c == '#')
            comment(l);
        else 
            errx(1, "lexer : unexpected token");
    }
}

/*function a little weird*/

/*pb : i want the parser to be able to request tokens to the lexer
 * but I  also want to write a peek() function for the parser, but
 * the upcoming token won't exist yet.
 * tried to do :
 *      when parser_advance is called : lex more than one token at a time.
 */

struct token *lexer_get_next_token(struct lexer* lexer)
{
    if (lexer_is_at_end(lexer))
    {
        struct token *t = calloc(1, sizeof(struct token));
        t -> type = END;
        return t;
    }

    lexer -> nb_tokens += 1;
    lexer -> start = lexer -> current;

    lexer_scan_tokens(lexer);
    return get_last_token(lexer -> t_list);
}

struct lexer *lexer_init(char *input)
{
    struct lexer *l = calloc(1, sizeof(struct lexer));
    l -> src = input;
    l -> src_len = strlen(l->src);
    l ->keywords = init_reserved_words();

    return l;
}

void lexer_lex(struct lexer *l)
{
    while (!lexer_is_at_end(l))
    {
        l -> start = l -> current;
        lexer_scan_tokens(l);
    }

    lexer_add_token_2(l, END);
}

#define HEHE
#ifdef HEHE

#define NB_TOKENS 38
char token_str[NB_TOKENS][20] =
{
    "&&", "||", ";;", "<<", ">>", "<&",
    ">&", "<>", "<<-", "|>",
    "if", "then", "else", "elif", "fi", "do", "done",
    "case", "esac", "while", "until", "for",
    "{", "}", "!", "in", "<", ">", ";", "NEWLINE",
    "WORD", "STRING", "|", "&", "NAME", "ASSIGNMENT_WORD",
    "(", ")"
};

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    struct lexer *l = lexer_init(argv[1]);

    while (!lexer_is_at_end(l))
    {
        l -> start = l -> current;
        lexer_scan_tokens(l);
    }

    struct token_list *tmp = l -> t_list;
    while (tmp)
    {
        if (tmp -> t -> type < NB_TOKENS)
            printf("%s [%s]\n",token_str[tmp -> t ->type], tmp->t->lexeme);
        tmp = tmp -> next;
    }

    lexer_free(l);
    return 0;
}

#endif
