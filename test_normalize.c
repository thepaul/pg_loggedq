#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "normalize.h"

void test_norm(const char* test, const char* result, int bufsize, int remove_const)
{
    char* b = malloc(bufsize);
    if (normalize_q(test, b, bufsize, remove_const) < 0)
    {
        perror("normalize_q");
        return;
    }
    if (strcmp(b, result))
        fprintf(stderr, "test_norm failure:\n?? %s ??\n !=\n?? %s ??\n", b, result);
}

int main()
{
    test_norm(
        "sELect  *  from \"FOObar\", BONGbar WHERE n=2++~3 -4.2; ",
        "select * from \"FOObar\" , bongbar where n = 2 ++~ 3 - 4.2 ;",
        1024, 0
    );

    test_norm(
        "sELect  *  from \"FOObar\", BONGbar WHERE n=2++~3 -4.2; ",
        "select * from \"FOObar\" , bongbar where n = ? ++~ ? - ? ;",
        1024, 1
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , 'once there was a ''snowman''' ) ;",
        1024, 0
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , ? ) ;",
        1024, 1
    );

    test_norm(
        "",
        "",
        1024, 0
    );

    test_norm(
        "",
        "",
        1024, 1
    );

    test_norm(
        "9 blah blah blah",
        "",
        1, 0
    );

    test_norm(
        "9 blah blah blah",
        "",
        1, 1
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into fo",
        15, 1
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , 'once there was a ''snowman'''",
        72, 0
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , 'once there was a ''snowman''",
        71, 0
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , 'once there was a ''snowman'",
        70, 0
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , 'once there was a ''snowman",
        69, 0
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , 'o",
        44, 0
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , ? ",
        44, 1
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , ?",
        43, 1
    );

    test_norm(
        "  insert  INTO foo(bar integer not null ,'once there was a ''snowman''' );",
        "insert into foo ( bar integer not null , ",
        42, 1
    );

    test_norm(
        "  insert  INTO foo(\"bar\" integer not null );",
        "insert into foo ( bar integer not null ) ;",
        1024, 1
    );

    test_norm(
        "  insert  INTO foo(\"bar\" integer not null );",
        "insert into foo ( bar integer not null ) ;",
        1024, 0
    );

    test_norm(
        "  insert  INTO foo(\"b\"\"ar\" integer not null );",
        "insert into foo ( \"b\"\"ar\" integer not null ) ;",
        1024, 0
    );

    return 0;
}
