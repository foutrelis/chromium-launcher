# chromium-launcher [![Build Status](https://travis-ci.org/foutrelis/chromium-launcher.svg?branch=master)](https://travis-ci.org/foutrelis/chromium-launcher) [![Coverage Status](https://coveralls.io/repos/github/foutrelis/chromium-launcher/badge.svg)](https://coveralls.io/github/foutrelis/chromium-launcher)

Chromium launcher with support for custom user flags.

## Usage

This launcher was originally written for the Arch Linux Chromium package.

It is meant to be installed as `/usr/bin/chromium` and act as a wrapper around
the Chromium binary.

Running `chromium --help` will show where it looks for the `chromium-flags.conf`
file, along with a list of the custom flags it was able to read from it.

## License

This project is licensed under ISC.
