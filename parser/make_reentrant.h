#ifndef make_reentrant_h
#define make_reentrant_h

#include "per_scanner_data.h"

#define GET_EXTRA                    per_scanner_data* extrad = yyget_extra(yyscanner)
#define xcdepth                      (yyextra->_xcdepth)
#define dolqstart                    (yyextra->_dolqstart)
#define lval                         (yyextra->_lval)
#define backslash_quote              (yyextra->_backslash_quote)
#define escape_string_warning        (yyextra->_escape_string_warning)
#define standard_conforming_strings  (yyextra->_standard_conforming_strings)
#define warn_on_first_escape         (yyextra->_warn_on_first_escape)
#define saw_high_bit                 (yyextra->_saw_high_bit)
#define yylloc                       (extrad->_yylloc)
#define literalbuf                   (extrad->_literalbuf)
#define literallen                   (extrad->_literallen)
#define literalalloc                 (extrad->_literalalloc)
#define scanbufhandle                (extrad->_scanbufhandle)
#define scanbuf                      (extrad->_scanbuf)

void _yyerror(const char* message, yyscan_t yyscanner);
#define yyerror(m) (_yyerror((m), yyscanner))

#undef SET_YYLLOC
#define SET_YYLLOC()  (yyextra->_yylloc = yytext - yyextra->_scanbuf)

#define addlit(t,n)   r_addlit((t),(n), yyscanner)
#define addlitchar(c) r_addlit((c), yyscanner)
#define litbufdup()   r_addlit(yyscanner)

#endif
