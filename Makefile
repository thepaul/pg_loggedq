# pg_loggedq scanner module

CFLAGS := -Wall -g
LEX := flex

scan.o: scan.c

scan.c: scan.l
	echo "#define _GNU_SOURCE" > $@.tmp
	$(LEX) -o $@ $^
	cat $@ >> $@.tmp
	mv $@.tmp $@

clean:
	$(RM) scan.o scan.c

.PHONY: clean
