# pg_loggedq scanner module

SCAN := parser/scan
CFLAGS := -Wall -g
LEX := flex
LEXOPTIONS := --header-file=$(SCAN).h

scan.so: $(SCAN).o
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(SCAN).o: CFLAGS+=-fPIC -D_GNU_SOURCE -I.
$(SCAN).o: $(SCAN).c

$(SCAN).c: $(SCAN).l
	$(LEX) $(LEXOPTIONS) -o $@ $^

parser/adapt.o: parser/adapt.c

test: test.o $(SCAN).o parser/adapt.o
	$(CC) $(LDOPTIONS) -o $@ $^

test.o: $(SCAN).c

clean:
	$(RM) $(SCAN).[och] parser/adapt.o scan.so

.PHONY: clean
