package main

import (
	"log"
	"os"
	"path/filepath"
	"syscall"
)

// These can be overridden in the Makefile
var (
	launcherVersion = "(unknown version)"
	chromiumName    = "chromium"
	chromiumBinary  = "/usr/lib/chromium/chromium"
	pepperFlashDir  = "/usr/lib/PepperFlash"
)

var (
	osExit      = os.Exit
	syscallExec = syscall.Exec
)

func main() {
	osExit(runLauncher())
}

func runLauncher() int {
	if len(os.Args) > 1 {
		switch os.Args[1] {
		case "-h", "--help":
			showHelp()
			return 0
		}
	}

	args := []string{chromiumBinary}
	args = append(args, getFlashFlags(pepperFlashDir)...)
	args = append(args, getUserFlags()...)
	args = append(args, os.Args[1:]...)

	wrapper, _ := filepath.Abs(os.Args[0])
	os.Setenv("CHROME_WRAPPER", wrapper)
	os.Setenv("CHROME_DESKTOP", chromiumName+".desktop")

	syscallExec(chromiumBinary, args, os.Environ())
	log.Println("Failed to execute", chromiumBinary)
	return 1
}
