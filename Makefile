PREFIX = /usr/local

CHROMIUM_SUFFIX  =
CHROMIUM_NAME    = chromium$(CHROMIUM_SUFFIX)
CHROMIUM_BIN     = /usr/lib/$(CHROMIUM_NAME)/$(CHROMIUM_NAME)
PEPPER_FLASH_DIR = /usr/lib/PepperFlash

override CFLAGS += $(shell pkg-config --cflags glib-2.0 json-glib-1.0)
override LDLIBS += $(shell pkg-config --libs glib-2.0 json-glib-1.0)
override CPPFLAGS += \
	-DLAUNCHER_VERSION=\"$(shell git describe)\" \
	-DCHROMIUM_NAME=\"$(CHROMIUM_NAME)\" \
	-DCHROMIUM_BIN=\"$(CHROMIUM_BIN)\" \
	-DPEPPER_FLASH_DIR=\"$(PEPPER_FLASH_DIR)\"

$(CHROMIUM_NAME): launcher.c config.h
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LDLIBS)

runtests: launcher_test.c minunit.h launcher.c config.h
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LDLIBS)

check: runtests
	./runtests

install: $(CHROMIUM_NAME)
	install -D $(CHROMIUM_NAME) "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

install-strip: install
	strip "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

uninstall:
	$(RM) "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

clean:
	$(RM) $(CHROMIUM_NAME) runtests *.o

.PHONY: check install install-strip uninstall clean
