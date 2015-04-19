OUT               := chromium-launcher launcher-errmsg

CHROMIUM_SUFFIX   :=
PREFIX            := /usr/local

CHROMIUM_NAME     := chromium$(CHROMIUM_SUFFIX)
CHROMIUM_BIN      := /usr/lib/$(CHROMIUM_NAME)/$(CHROMIUM_NAME)
PEPPER_FLASH_DIR  := /usr/lib/PepperFlash

LAUNCHER_LIBDIR   := $(PREFIX)/lib/$(CHROMIUM_NAME)-launcher
LAUNCHER_ERRMSG   := $(LAUNCHER_LIBDIR)/launcher-errmsg

override CFLAGS   += -std=c99 $(shell pkg-config --cflags gtk+-2.0)
override LDLIBS   += $(shell pkg-config --libs gtk+-2.0)

all: $(OUT)

chromium-launcher: chromium-launcher.in
	sed \
		-e "s|@CHROMIUM_NAME@|$(CHROMIUM_NAME)|g" \
		-e "s|@CHROMIUM_BIN@|$(CHROMIUM_BIN)|g" \
		-e "s|@PEPPER_FLASH_DIR@|$(PEPPER_FLASH_DIR)|g" \
		-e "s|@LAUNCHER_ERRMSG@|$(LAUNCHER_ERRMSG)|g" \
		$< >$@

install: all
	install -Dm755 chromium-launcher "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"
	install -Dm755 launcher-errmsg "$(DESTDIR)$(LAUNCHER_ERRMSG)"

uninstall:
	$(RM) "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"
	$(RM) "$(DESTDIR)$(LAUNCHER_ERRMSG)"
	rmdir --ignore-fail-on-non-empty "$(DESTDIR)$(LAUNCHER_LIBDIR)"

clean:
	$(RM) $(OUT)

.PHONY: all clean install uninstall
