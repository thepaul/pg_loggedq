# pg_loggedq scanner module

CFLAGS := -Wall -g
LEX := flex

scan.so: scan.o
	$(CC) $(LDFLAGS) -shared -o $@ $^

scan.o: CFLAGS+=-fPIC -D_GNU_SOURCE
scan.o: scan.c

scan.c: scan.l
	$(LEX) -o $@ $^

clean:
	$(RM) scan.o scan.c scan.h scan.so

.PHONY: clean
