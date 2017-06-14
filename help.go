package main

import (
	"fmt"
)

func showHelp() {
	flashFlags := fmt.Sprintf("%q", getFlashFlags(pepperFlashDir))
	userFlags := fmt.Sprintf("%q", getUserFlags())

	fmt.Println(`
Chromium launcher ` + launcherVersion + ` -- for Chromium help, see ` + chromiumName + `(1)

Custom flags are read from the following file:

  ` + getUserFlagsFilePath() + `

Arguments contained in that file are split on whitespace and shell quoting
rules apply but no further parsing is performed. Lines starting with a hash
symbol (#) are skipped. Lines with unbalanced quotes are skipped as well.

Currently detected flags:

  ` + userFlags + `
`)

	if flashFlags != "[]" {
		fmt.Println(`Flags automatically added for PepperFlash support:

  ` + flashFlags + `
`)
	}
}
