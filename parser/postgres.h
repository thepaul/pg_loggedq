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

typedef size_t Size;

/* pg default is 64, but we want to accept everything we can */
#define NAMEDATALEN 1024

#define ERROR 1
#define WARNING 2

#define pstrdup(s) (strdup(s))
#define palloc(n) (calloc(n, 1))
#define pfree(x) (free(x))
#define repalloc(b,n) (realloc(b,n))

#include <assert.h>
#define Assert assert

#define PG_ENCODING_IS_CLIENT_ONLY(encoding) 0

#define ERRCODE_NONSTANDARD_USE_OF_ESCAPE_CHARACTER 1
#define ERRCODE_SYNTAX_ERROR                        2

#define _(a) (a)

#endif
