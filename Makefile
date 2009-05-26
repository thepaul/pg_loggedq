# pg_loggedq scanner module

CFLAGS := -Wall -g
LEX := flex

SCAN := parser/scan

scan.so: $(SCAN).o
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(SCAN).o: CFLAGS+=-fPIC -D_GNU_SOURCE -I.
$(SCAN).o: $(SCAN).c

$(SCAN).c: $(SCAN).l
	$(LEX) -o $@ $^

clean:
	$(RM) $(SCAN).[och] scan.so

.PHONY: clean
