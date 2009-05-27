#include <stdio.h>
#include "parser/scan.h"
#include "parser/postgres.h"
#include "parser/gramparse.h"

void scanner_init(const char* sql);

extern YYSTYPE yylval;
extern int seen_error;

int main()
{
    int res;

    scanner_init("select f from bar WHERE 9 = 'f''o\'o' and \"MO\"\"NKey\" + $6::int *~~& -24;");

    do {
        fprintf(stderr, "checking yylex\n");
        res = base_yylex();
        switch (res)
        {
            case BCONST:
            case XCONST:
            case FCONST:
            case SCONST:
                printf("constant %s replaced with ?\n", yylval.str);
                free(yylval.str);
                break;
            case ICONST:
                printf("constant (iconst) %ld replaced with ?\n", yylval.ival);
                break;
            case TYPECAST:
                printf("TYPECAST ::\n");
                break;
            case PARAM:
                printf("PARAM %ld\n", yylval.ival);
                break;
            case IDENT:
                printf("IDENT %s\n", yylval.str);
                free(yylval.str);
                break;
            case Op:
                printf("Op %s\n", yylval.str);
                free(yylval.str);
                break;
            case KEYWORD:
                printf("KEYWORD %s\n", yylval.keyword);
                free(yylval.keyword);
                break;
            case 0:
                break;
            default:
                printf("CHAR '%c' (keep intact)\n", res);
        }
        if (seen_error)
            break;
    } while (res);

    return 0;
}
