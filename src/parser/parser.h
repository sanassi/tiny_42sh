#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <err.h>

#include "lexer/lexer.h"

struct parser
{
    struct token_list *tokens;
    size_t current;
};



#endif
