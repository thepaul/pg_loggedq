/* adapt.c */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "postgres.h"
#include "gramparse.h"

int seen_error = 0;

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
