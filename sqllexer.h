/* sqllexer.h */

#ifndef SQLLEXER_H
#define SQLLEXER_H

struct _per_scanner_data;
typedef struct _per_scanner_data per_scanner_data;

typedef char YYSTYPE;

#ifndef YY_CURRENT_BUFFER
#include "scan.h"
#endif

#define CONSTANT_REPLACED 1
#define NEXT_TOKEN 2
#define SEND_IT_BACK lval = strndup(yytext, yyleng); return NEXT_TOKEN

struct _per_scanner_data {
    int		_xcdepth;		/* depth of nesting in slash-star comments */
    char    *_dolqstart;    /* current $foo$ quote start string */
    int		_yylloc;
    char	*_lval;

    /*
     * literalbuf is used to accumulate literal values when multiple rules
     * are needed to parse a single literal.  Call startlit to reset buffer
     * to empty, addlit to add text.  Note that the buffer is calloc'd and
     * starts life afresh on every parse cycle.
     */
    char	*_literalbuf;	/* expandable buffer */
    int		_literallen;	/* actual current length */
    int		_literalalloc;	/* current allocated buffer size */

    /* Handles to the buffer that the lexer uses internally */
    YY_BUFFER_STATE _scanbufhandle;
    char *_scanbuf;

};

#endif
