# chromium-launcher ![ci](https://github.com/foutrelis/chromium-launcher/workflows/ci/badge.svg) [![Coverage](https://codecov.io/gh/foutrelis/chromium-launcher/branch/master/graph/badge.svg)](https://codecov.io/gh/foutrelis/chromium-launcher)

Chromium launcher with support for custom user flags.

## Usage

This launcher was originally written for the Arch Linux Chromium package.

It is meant to be installed as `/usr/bin/chromium` and act as a wrapper around
the Chromium binary.

Running `chromium --help` will show where it looks for the `chromium-flags.conf`
file, along with a list of the custom flags it was able to read from it.

## License

This project is licensed under ISC.
