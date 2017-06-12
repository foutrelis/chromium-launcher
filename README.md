# chromium-launcher [![Build Status](https://travis-ci.org/foutrelis/chromium-launcher.svg?branch=master)](https://travis-ci.org/foutrelis/chromium-launcher)

Chromium launcher with support for Pepper Flash and custom user flags.

## Requirements

A derpy looking Gopher to compile this with. (Tested with Go 1.8.)

## Usage

This launcher was originally written for the Arch Linux Chromium package.

It is meant to be installed as `/usr/bin/chromium` and act as a wrapper around
the Chromium binary.

Running `chromium --help` will show where it looks for the `chromium-flags.conf`
file, along with a list of the custom flags it was able to read from it. If
PepperFlash was found on the system, it will generate and display flags for
that as well.

## License

This project is licensed under ISC.
