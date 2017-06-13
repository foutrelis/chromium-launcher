GOPATH = $(shell pwd)/vendor:$(shell pwd)
export GOPATH

PREFIX = /usr/local

CHROMIUM_SUFFIX  =
CHROMIUM_NAME    = chromium$(CHROMIUM_SUFFIX)
CHROMIUM_BIN     = /usr/lib/$(CHROMIUM_NAME)/$(CHROMIUM_NAME)
PEPPER_FLASH_DIR = /usr/lib/PepperFlash

VERSION = $(shell git describe)
LDFLAGS = -ldflags "\
	-X launcher.Version=$(VERSION) \
	-X launcher.ChromiumBinary=$(CHROMIUM_BIN) \
	-X launcher.ChromiumName=$(CHROMIUM_NAME) \
	-X launcher.PepperFlashDir=$(PEPPER_FLASH_DIR)"

SRC = $(shell find . -type f -name '*.go')

$(CHROMIUM_NAME): $(SRC)
	go build $(LDFLAGS) -o $(CHROMIUM_NAME) ./src/main

check:
	go test -cover ./src/...

install: $(CHROMIUM_NAME)
	install -D $(CHROMIUM_NAME) "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

install-strip: install
	strip "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

uninstall:
	$(RM) "$(DESTDIR)$(PREFIX)/bin/$(CHROMIUM_NAME)"

clean:
	$(RM) $(CHROMIUM_NAME)

.PHONY: check install install-strip uninstall clean
