/* normalize.c */

#define _ISOC99_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "parser/adapt.h"
#include "parser/gramparse.h"
#include "parser/per_scanner_data.h"

static int
writestr_escape(char* buf, int buflen, const char* src, char doubleme);

/**
 * Normalizes an SQL statement.
 *
 * Takes a postgreSQL-compatible SQL statement or query and returns
 * it in a normalized form. Two such statements which differ only in
 * whitespace or equivalent expressions of the same constants or
 * identifiers will yield the same output.
 *
 * Also optionally replaces any constants in the statement (string,
 * numeric, bitstrings, etc) with a question mark ('?').
 *
 * @param sql The input statement.
 * @param buf The output buffer.
 * @param buflen The allocated length of the output buffer. This function
 *            will not write past this many bytes. If there is not enough
 *            room for the output, the output will be truncated. The
 *            resulting output string will always be nul-terminated when
 *            buflen > 0.
 * @param remove_const If true, all constant tokens in the input SQL will be
 *            replaced with '?'.
 *
 * @returns 0 if everything was successful,
 *          1 if the output was truncated because the output buffer was too
 *            small, or
 *         -1 if there was some other error (errno should be set).
 */
int
normalize_q(const char* sql, char* buf, unsigned int buflen, int remove_const)
{
    char* p = buf;
    char* bend = buf + buflen;
    char* freeme = NULL;
    yyscan_t scanner;
    int token;
    int s;
    int ret = 0;
    YYSTYPE* tokval;

    if (buflen <= 0 || sql == NULL || buf == NULL)
    {
        errno = EINVAL;
        return -1;
    }

#define addchar(c)                                        \
    ({                                                    \
        if (p >= bend)                                    \
        {                                                 \
            *(bend - 1) = '\0';                           \
            ret = 1;                                      \
            goto error_out;                               \
        }                                                 \
        else                                              \
        {                                                 \
            *p++ = (c);                                   \
        }                                                 \
    })

#define addnum(n)                                         \
    ({                                                    \
        if (p >= bend)                                    \
        {                                                 \
            *(bend - 1) = '\0';                           \
            ret = 1;                                      \
            goto error_out;                               \
        }                                                 \
        else                                              \
        {                                                 \
            s = snprintf(p, bend - p, "%ld", (n));        \
            if (s < 0)                                    \
            {                                             \
                *p = '\0';                                \
                ret = -1;                                 \
                goto error_out;                           \
            }                                             \
            p += s;                                       \
        }                                                 \
    })

#define addstr(str)                                       \
    ({                                                    \
        if (p >= bend)                                    \
        {                                                 \
            *(bend - 1) = '\0';                           \
            ret = 1;                                      \
            goto error_out;                               \
        }                                                 \
        else                                              \
        {                                                 \
            s = snprintf(p, bend - p, "%s", (str));       \
            if (s < 0)                                    \
            {                                             \
                *p = '\0';                                \
                ret = -1;                                 \
                goto error_out;                           \
            }                                             \
            p += s;                                       \
        }                                                 \
    })

#define addstr_escape(str,q)                              \
    ({                                                    \
        if (p >= bend)                                    \
        {                                                 \
            *(bend - 1) = '\0';                           \
            ret = 1;                                      \
            goto error_out;                               \
        }                                                 \
        else                                              \
        {                                                 \
            s = writestr_escape(p, bend - p, (str), (q)); \
            if (s < 0)                                    \
            {                                             \
                *p = '\0';                                \
                ret = -1;                                 \
                goto error_out;                           \
            }                                             \
            p += s;                                       \
        }                                                 \
    })

    scanner = lexer_init(sql);
    tokval = lexer_lval(scanner);
    token = lexer_next(scanner);
    while (token != 0)
    {
        switch (token)
        {
        case ICONST:
            if (remove_const)
                addchar('?');
            else
                addnum(tokval->ival);
            break;

        case PARAM:
            addchar('$');
            addnum(tokval->ival);
            break;

        case TYPECAST:
            addchar(':');
            addchar(':');
            break;

        case BCONST:
            freeme = tokval->str;
            if (remove_const)
            {
                addchar('?');
            }
            else
            {
                addchar('b');
                addchar('\'');
                addstr_escape(tokval->str, '\'');
                addchar('\'');
            }
            break;

        case XCONST:
            freeme = tokval->str;
            if (remove_const)
            {
                addchar('?');
            }
            else
            {
                addchar('x');
                addchar('\'');
                addstr_escape(tokval->str, '\'');
                addchar('\'');
            }
            break;

        case FCONST:
            freeme = tokval->str;
            if (remove_const)
            {
                addchar('?');
            }
            else
            {
                addstr(tokval->str);
            }
            break;

        case SCONST:
            freeme = tokval->str;
            if (remove_const)
            {
                addchar('?');
            }
            else
            {
                addchar('\'');
                addstr_escape(tokval->str, '\'');
                addchar('\'');
            }
            break;

        case KEYWORD:
            freeme = tokval->keyword;
            addstr(tokval->keyword);
            break;

        case IDENT:
            {
                char* c;
                int need_dq = 0;
                freeme = tokval->str;
                for (c = tokval->str; *c; ++c)
                {
                    if (isupper(*c) || *c == '"')
                        need_dq = 1;
                }
                if (need_dq)
                {
                    addchar('"');
                    addstr_escape(tokval->str, '"');
                    addchar('"');
                }
                else
                {
                    addstr(tokval->str);
                }
            }
            break;

        case Op:
            freeme = tokval->str;
            addstr(tokval->str);
            break;

        default:
            addchar(token);
            break;
        }
        if (freeme)
        {
            free(freeme);
            freeme = NULL;
        }
        addchar(' ');
        token = lexer_next(scanner);
    }

    if (p > buf)
    {
        /* a space was output last- we don't want it */
        p--;
    }
    if (p >= bend)
    {
        *(bend - 1) = '\0';
        return 1;
    }
    *p = '\0';
    return 0;

error_out:
    if (freeme)
        free(freeme);
    return ret;
}

static int
writestr_escape(char* buf, int buflen, const char* src, char doubleme)
{
    const char* psrc;
    char* p = buf;
    char* bend = buf + buflen;

    if (buf == NULL || src == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if (buflen == 0)
    {
        if (src[0] == '\0')
            return 0;
        else
            return 1;
    }

    for (psrc = src; *psrc; ++psrc)
    {
        if (*psrc == doubleme)
        {
            if (p >= bend - 2)
            {
                if (p < bend)
                    *p = '\0';
                else
                    *(bend - 1) = '\0';
                return buflen + 1;  /* anything beyond buflen will do */
            }
            *p++ = doubleme;
            *p++ = doubleme;
        }
        else
        {
            if (p >= bend - 1)
            {
                *(bend - 1) = '\0';
                return buflen + 1;
            }
            *p++ = *psrc;
        }
    }
    *p = '\0';
    return p - buf;
}
