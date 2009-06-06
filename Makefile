# pg_loggedq scanner module

SCAN := parser/scan
CFLAGS := -Wall -g -fPIC -I.
LEX := flex
LEXOPTIONS := --header-file=parser/scan.h -R

libsqlscanner.a: parser/scan.o parser/adapt.o
	$(AR) r $@ $^

parser/scan.o: CFLAGS+=-D_GNU_SOURCE -Wno-unused-variable -Wno-unused-function

parser/scan.c: parser/scan.l
	$(LEX) $(LEXOPTIONS) -o $@ $^

test: test.o parser/scan.o parser/adapt.o
	$(CC) $(LDOPTIONS) -o $@ $^

clean:
	$(RM) parser/scan.[och] parser/adapt.o test.o test libsqlscanner.a
	$(RM) -r build

.PHONY: clean
