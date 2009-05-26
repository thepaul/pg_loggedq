#ifndef dummy_postgres_h
#define dummy_postgres_h

typedef enum {
    BACKSLASH_QUOTE_OFF,
    BACKSLASH_QUOTE_SAFE_ENCODING,
} BackslashQuoteType;

typedef enum {
    false = 0,
    true = 1
} bool;

int yylloc;

void yyerror(const char* message);

typedef union {
    char* str;
    char* keyword;
    long int ival;
} YYSTYPE;

YYSTYPE yylval;

#endif
