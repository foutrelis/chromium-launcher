package launcher

import (
	"log"
	"os"
	"path/filepath"
	"syscall"
)

var execCommand = syscall.Exec

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
