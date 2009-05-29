#ifndef make_reentrant_h
#define make_reentrant_h

#include "per_scanner_data.h"

#ifdef YY_EXTRA_TYPE
#undef YY_EXTRA_TYPE
#endif

#define YY_EXTRA_TYPE per_scanner_data*

#define GET_EXTRA    struct yyguts_t* yyg = (struct yyguts_t*)yyscanner

#define xcdepth                      (yyextra->_xcdepth)
#define dolqstart                    (yyextra->_dolqstart)
#define yylval                       (yyextra->_yylval)
#define backslash_quote              (yyextra->_backslash_quote)
#define escape_string_warning        (yyextra->_escape_string_warning)
#define standard_conforming_strings  (yyextra->_standard_conforming_strings)
#define warn_on_first_escape         (yyextra->_warn_on_first_escape)
#define saw_high_bit                 (yyextra->_saw_high_bit)
#define yylloc                       (yyextra->_yylloc)
#define literalbuf                   (yyextra->_literalbuf)
#define literallen                   (yyextra->_literallen)
#define literalalloc                 (yyextra->_literalalloc)
#define scanbufhandle                (yyextra->_scanbufhandle)
#define scanbuf                      (yyextra->_scanbuf)

void _yyerror(const char* message, yyscan_t yyscanner);
#define yyerror(m) (_yyerror((m), yyscanner))

static void r_addlit(char* ytext, int yleng, yyscan_t yyscanner);
static void r_addlitchar(unsigned char c, yyscan_t yyscanner);
static char* r_litbufdup(yyscan_t yyscanner);
static int r_lexer_errposition(yyscan_t yyscanner);
static unsigned char r_unescape_single_char(unsigned char c, yyscan_t yyscanner);
static void r_check_string_escape_warning(unsigned char c, yyscan_t yyscanner);
static void r_check_escape_warning(yyscan_t yyscanner);

#define addlit(t,n)   r_addlit((t),(n), yyscanner)
#define addlitchar(c) r_addlitchar((c), yyscanner)
#define litbufdup()   r_litbufdup(yyscanner)
#define lexer_errposition() r_lexer_errposition(yyscanner)
#define unescape_single_char(c) r_unescape_single_char((c), yyscanner)
#define check_string_escape_warning(c) r_check_string_escape_warning((c), yyscanner)
#define check_escape_warning() r_check_escape_warning(yyscanner)

#ifdef startlit
#undef startlit
#endif
#define startlit() (yyextra->_literalbuf[0] = '\0', yyextra->_literallen = 0)

#endif
