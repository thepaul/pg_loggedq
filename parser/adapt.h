#ifndef adapt_h
#define adapt_h

#include "per_scanner_data.h"
#include "postgres.h"
#include "scan.h"

int scanner_init(const char* str, yyscan_t yyscanner);
int scanner_finish(yyscan_t yyscanner);

yyscan_t lexer_init(const char* sql);
void lexer_destroy(yyscan_t yyscanner);
#define lexer_next(y) base_yylex(y)
YYSTYPE* lexer_lval(yyscan_t yyscanner);

#endif
