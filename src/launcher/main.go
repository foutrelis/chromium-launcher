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

func main()  {
	if len(os.Args) > 1 {
		switch os.Args[1] {
		case "-h", "--help":
			showHelp()
			os.Exit(0)
		}
	}

	args := []string{ChromiumBinary}
	args = append(args, getFlashFlags(PepperFlashDir)...)
	args = append(args, getUserFlags()...)
	args = append(args, os.Args[1:]...)

	wrapper, _ := filepath.Abs(os.Args[0])
	os.Setenv("CHROME_WRAPPER", wrapper)
	os.Setenv("CHROME_DESKTOP", "chromium.desktop")

	syscall.Exec(ChromiumBinary, args, os.Environ())
	log.Fatal("Failed to execute " + ChromiumBinary)

}
