# pg_loggedq scanner module

SCAN := parser/scan
CFLAGS := -Wall -g -Wno-unused-variable -Wno-unused-function
LEX := flex
LEXOPTIONS := --header-file=parser/scan.h -R

libsqlscanner.a: parser/scan.o parser/adapt.o
	$(AR) r $@ $^

parser/scan.o: CFLAGS+=-D_GNU_SOURCE -I.
parser/scan.o: parser/scan.c

parser/scan.c: parser/scan.l
	$(LEX) $(LEXOPTIONS) -o $@ $^

test: test.o parser/scan.o parser/adapt.o
	$(CC) $(LDOPTIONS) -o $@ $^

clean:
	$(RM) parser/scan.[och] parser/adapt.o test.o test libsqlscanner.a
	$(RM) -r build

.PHONY: clean
