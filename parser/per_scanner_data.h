/* sqllexer.h */

#ifndef per_scanner_data_h
#define per_scanner_data_h

#ifdef YY_EXTRA_TYPE
#undef YY_EXTRA_TYPE
#endif

#define YY_EXTRA_TYPE per_scanner_data*

struct _per_scanner_data;
typedef struct _per_scanner_data per_scanner_data;

#ifndef YY_CURRENT_BUFFER
#include "scan.h"
#endif

#include "postgres.h"

struct _per_scanner_data {
    int		_xcdepth;		/* depth of nesting in slash-star comments */
    char    *_dolqstart;    /* current $foo$ quote start string */
    int		_yylloc;
    YYSTYPE _yylval;
    BackslashQuoteType _backslash_quote;
    bool    _escape_string_warning;
    bool    _standard_conforming_strings;
    bool    _warn_on_first_escape;
    bool    _saw_high_bit;

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
