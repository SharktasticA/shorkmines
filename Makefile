SYSROOT ?=
CFLAGS = -std=c99 -Wall
LDLIBS = -lncursesw -lminesweeper
LDFLAGS = -Llibminesweeper -L$(SYSROOT)/lib
CPPFLAGS = -Ilibminesweeper/include -I$(SYSROOT)/include

ifdef EMBEDDED
	CFLAGS += -DEMBEDDED
endif

PREFIX?=/usr
EXECUTABLE = shorkmines
LIBMINESWEEPER = libminesweeper/libminesweeper.a
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

$(EXECUTABLE): $(LIBMINESWEEPER) $(OBJECTS)

$(LIBMINESWEEPER):
	$(MAKE) -C libminesweeper SYSROOT=$(SYSROOT)

.PHONY: clean install uninstall

clean:
	rm -f $(EXECUTABLE) || true
	rm $(OBJECTS) || true
	$(MAKE) -C libminesweeper clean || true

install: $(EXECUTABLE)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -s $(EXECUTABLE) $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/share/man/man1/
	install man/$(EXECUTABLE).1 $(DESTDIR)$(PREFIX)/share/man/man1/

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE)
	rm $(DESTDIR)$(PREFIX)/share/man/man1/$(EXECUTABLE).1
