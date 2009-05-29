#ifndef adapt_h
#define adapt_h

#include "per_scanner_data.h"
#include "postgres.h"
#include "scan.h"

int yylex_init_extra(YY_EXTRA_TYPE extra, yyscan_t* ptr_yy_globals);
int yylex(yyscan_t yyscanner);
int yylex_destroy(yyscan_t yyscanner);
int scanner_init(const char* str, yyscan_t yyscanner);
int scanner_finish(yyscan_t yyscanner);

yyscan_t lexer_init(const char* sql);
void lexer_destroy(yyscan_t yyscanner);

#endif
