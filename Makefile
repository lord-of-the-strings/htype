CC      = gcc
CFLAGS  = -O2 -Wall -Wextra
DBGFLAGS= -g -O0 -Wall -Wextra -DDEBUG
PREFIX  = /usr/local
BINARY  = htype
SRC     = htype.c
MAN1    = htype.1

.PHONY: all debug install uninstall clean

all: $(BINARY)

$(BINARY): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

debug: $(SRC)
	$(CC) $(DBGFLAGS) -o $(BINARY)-debug $

install: $(BINARY)
	install -Dm755 $(BINARY) $(PREFIX)/bin/$(BINARY)
	install -Dm644 $(MAN1)   $(PREFIX)/share/man/man1/$(MAN1)

uninstall:
	rm -f $(PREFIX)/bin/$(BINARY)
	rm -f $(PREFIX)/share/man/man1/$(MAN1)

clean:
	rm -f $(BINARY) $(BINARY)-debug
