#include <stdio.h>
#include "sqllexer.h"

void scanner_init(const char* sql, void* yyscanner);

int main()
{
    void* yyscanner;
    per_scanner_data d;
    int res;

    base_yylex_init_extra(&d, &yyscanner);
    scanner_init("select foo from bar where 9 = 'foo';", yyscanner);

    do {
        res = base_yylex("", yyscanner);
        if (res == CONSTANT_REPLACED)
            printf("?");
        else if (res == NEXT_TOKEN)
            printf("%s", d._lval);
    } while (res);
    printf("\n");

    return 0;
}
