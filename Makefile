PREFIX ?= /usr

CHROMIUM_SUFFIX  =
CHROMIUM_NAME    = chromium$(CHROMIUM_SUFFIX)
CHROMIUM_BINARY  = $(PREFIX)/lib/$(CHROMIUM_NAME)/$(CHROMIUM_NAME)
CHROMIUM_VENDOR  = $(shell . /etc/os-release; echo $$NAME)

override CFLAGS += $(shell pkg-config --cflags glib-2.0)
override LDLIBS += $(shell pkg-config --libs glib-2.0)
override CPPFLAGS += \
	-DLAUNCHER_VERSION=\"$(shell git describe 2>/dev/null || echo v8)\" \
	-DCHROMIUM_NAME=\"$(CHROMIUM_NAME)\" \
	-DCHROMIUM_BINARY=\"$(CHROMIUM_BINARY)\" \
	-DCHROMIUM_VENDOR="\"$(CHROMIUM_VENDOR)\""

ifeq ($(ENABLE_GCOV),1)
	RUNTESTS_CFLAGS = -fprofile-arcs -ftest-coverage
endif

$(CHROMIUM_NAME): launcher.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LDLIBS)

runtests: launcher_test.c minunit.h launcher.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(RUNTESTS_CFLAGS) $(LDFLAGS) $< $(LDLIBS)

check: runtests
	./runtests

install: $(CHROMIUM_NAME)
	install -D $(CHROMIUM_NAME) "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

install-strip: install
	strip "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

uninstall:
	$(RM) "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

clean:
	$(RM) $(CHROMIUM_NAME) runtests *.o *.{gcda,gcno,gcov}

.PHONY: check install install-strip uninstall clean
