/* adapt.c */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "postgres.h"
#include "gramparse.h"
#include "scan.h"
#include "per_scanner_data.h"
#include "adapt.h"

int seen_error = 0;

yyscan_t
lexer_init(const char* sql)
{
    per_scanner_data extra;
    yyscan_t scanner;
    int rc;

    extra._xcdepth = 0;
    extra._dolqstart = NULL;
    extra._backslash_quote = BACKSLASH_QUOTE_SAFE_ENCODING;
    extra._escape_string_warning = true;
    extra._standard_conforming_strings = false;
    extra._warn_on_first_escape = false;
    extra._saw_high_bit = false;
    extra._literalbuf = NULL;
    extra._literallen = 0;
    extra._literalalloc = 0;
    extra._scanbufhandle = NULL;
    extra._scanbuf = NULL;

    if (yylex_init_extra(&extra, &scanner) != 0)
        return NULL;
    scanner_init(sql, scanner);
    return scanner;
}

void
lexer_destroy(yyscan_t scanner)
{
    scanner_finish(scanner);
    yylex_destroy(scanner);
}

char*
errmsg(const char* fmt, ...)
{
    va_list ap;
    char* msg;

    va_start(ap, fmt);
    vasprintf(&msg, fmt, ap);
    va_end(ap);

    return msg;
}

void
report_error(int code, const char* message, ...)
{
    seen_error = 1;
    fprintf(stderr, "REPORT_ERROR code %d: %s\n", code, message);
}

static ScanKeyword scan_keyword;

ScanKeyword*
ScanKeywordLookup(const char* name)
{
    scan_keyword.name = strdup(name);
    scan_keyword.value = KEYWORD;
    return &scan_keyword;
}

void
truncate_identifier(char* str, int len, int mongoose)
{
    if (len > (NAMEDATALEN - 1))
        str[NAMEDATALEN - 1] = '\0';
}

char*
downcase_truncate_identifier(const char* str, int len, int mongoose)
{
    char* result;
    char* s;

    result = strndup(str, NAMEDATALEN - 1);
    for (s = result; *s; ++s)
        *s = tolower(*s);
    return result;
}
