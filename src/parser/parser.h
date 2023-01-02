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



#endif
