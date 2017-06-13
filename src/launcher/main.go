package main

import (
	"log"
	"os"
	"path/filepath"
	"syscall"
)

// These should be set by the Makefile but also set sane default values
var Version = "(unknown version)"
var ChromiumName = "chromium"
var ChromiumBinary = "/usr/lib/chromium/chromium"
var PepperFlashDir = "/usr/lib/PepperFlash"

var execCommand = syscall.Exec

func main() {
	os.Exit(RunLauncher())
}

func RunLauncher() int {
	if len(os.Args) > 1 {
		switch os.Args[1] {
		case "-h", "--help":
			showHelp()
			return 0
		}
	}

	args := []string{ChromiumBinary}
	args = append(args, getFlashFlags(PepperFlashDir)...)
	args = append(args, getUserFlags()...)
	args = append(args, os.Args[1:]...)

	wrapper, _ := filepath.Abs(os.Args[0])
	os.Setenv("CHROME_WRAPPER", wrapper)
	os.Setenv("CHROME_DESKTOP", ChromiumName + ".desktop")

	execCommand(ChromiumBinary, args, os.Environ())
	log.Println("Failed to execute", ChromiumBinary)
	return 1
}
