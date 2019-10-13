# This file is part of mn - simple metronome.
# See LICENSE For copyright and license information.

CPPFLAGS= -D_DEFAULT_SOURCE -D_FORTIFY_SOURCE=2
CFLAGS=-std=c99 -fpie -fno-plt -march=native -mtune=native -Wall -Wextra -Wpedantic
LDFLAGS=-pie

ifdef SANITIZE
	CFLAGS+= -fsanitize=$(SANITIZE)
	LDFLAGS+= -fsanitize=$(SANITIZE)
endif

ifdef DEBUG
	CFLAGS+= -Og -g
else
	CPPFLAGS+= -DNDEBUG
	CFLAGS+= -Os
endif

install: CPPFLAGS+= -DMN_TICK_FILE_PATH=\"$(DESTDIR)$(PREFIX)/share/mn/tick.u8\"

BUILDDIR=build

all: mn tick.u8

mn: $(BUILDDIR)/mn.o $(BUILDDIR)/internals.o $(BUILDDIR)/sound.o
	$(CC) $(LDFLAGS) -lpulse -lpulse-simple $^ -o $@

tick.u8: makechirp
	./makechirp > $@

makechirp: $(BUILDDIR)/makechirp.o
	$(CC) $(LDFLAGS) -lm $^ -o $@

$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
	rm -f mn makechirp tick.u8

install: mn tick.u8
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f mn $(DESTDIR)$(PREFIX)/bin/mn
	chmod 755 $(DESTDIR)$(PREFIX)/bin/mn
	mkdir -p $(DESTDIR)$(PREFIX)/share/mn
	cp -f tick.u8 $(DESTDIR)$(PREFIX)/share/mn/tick.u8
	chmod 644 $(DESTDIR)$(PREFIX)/share/mn/tick.u8

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/mn
	rm -f $(DESTDIR)$(PREFIX)/share/mn/tick.u8
	rmdir $(DESTDIR)$(PREFIX)/share/mn/

.PHONY: all clean install uninstall
