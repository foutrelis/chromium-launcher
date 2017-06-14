package main

import (
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"reflect"
	"strings"
	"testing"
)

func TestMain(t *testing.T) {
	defer func() {
		log.SetOutput(os.Stderr)
	}()
	log.SetOutput(ioutil.Discard)

	runMainHelpTestCase(t, "-h")
	runMainHelpTestCase(t, "--help")

	runMainExecTestCase(t, []string{})
	runMainExecTestCase(t, []string{"--test-flag"})
}

func runMainHelpTestCase(t *testing.T, flag string) {
	oldPepperFlashDir := pepperFlashDir
	oldExecCommand := execCommand
	oldArgs := os.Args
	oldStdout := os.Stdout
	defer func() {
		pepperFlashDir = oldPepperFlashDir
		execCommand = oldExecCommand
		os.Args = oldArgs
		os.Stdout = oldStdout
	}()

	pepperFlashDir = "testdata"
	execCommand = func(string, []string, []string) error { return nil }
	os.Args = []string{"RunLauncher()", flag}
	r, w, _ := os.Pipe()
	os.Stdout = w

	if err := RunLauncher(); err != 0 {
		t.Fatalf("%s exited with status code %d instead of 0", os.Args, err)
	}

	w.Close()
	out, _ := ioutil.ReadAll(r)

	for _, expected := range []string{"Currently detected flags", "PepperFlash support"} {
		if !strings.Contains(string(out), expected) {
			t.Fatalf("Can't find %q in --help output", expected)
		}
	}
}

func runMainExecTestCase(t *testing.T, flags []string) {
	oldPepperFlashDir := pepperFlashDir
	oldExecCommand := execCommand
	oldArgs := os.Args
	oldConfigHome := os.Getenv("XDG_CONFIG_HOME")
	defer func() {
		pepperFlashDir = oldPepperFlashDir
		execCommand = oldExecCommand
		os.Args = oldArgs
		os.Setenv("XDG_CONFIG_HOME", oldConfigHome)
	}()

	pepperFlashDir = "testdata"
	var execArgs []string
	execCommand = func(argv0 string, argv []string, envv []string) error {
		execArgs = []string{argv0}
		execArgs = append(execArgs, argv...)
		return nil
	}
	os.Args = []string{"RunLauncher()"}
	os.Args = append(os.Args, flags...)
	os.Setenv("XDG_CONFIG_HOME", "testdata")

	expectedArgs := []string{
		chromiumBinary, chromiumBinary, "--ppapi-flash-version=4.5.6",
		"--ppapi-flash-path=testdata/libpepflashplayer.so"}
	expectedArgs = append(expectedArgs, "--if", "--it", "--builds", "--it --ships")
	expectedArgs = append(expectedArgs, flags...)

	if err := RunLauncher(); err != 1 {
		t.Fatalf("%s exited with status code %d instead of 1", os.Args, err)
	}

	if !reflect.DeepEqual(execArgs, expectedArgs) {
		t.Fatalf("Wrong args passed to Exec\ngot: %#v\nexpected: %#v", execArgs, expectedArgs)
	}

	wrapper, _ := filepath.Abs(os.Args[0])
	checkEnvVar(t, "CHROME_DESKTOP", chromiumName+".desktop")
	checkEnvVar(t, "CHROME_WRAPPER", wrapper)
}

func checkEnvVar(t *testing.T, name string, expected string) {
	if envVar := os.Getenv(name); envVar != expected {
		t.Fatalf("%s env var is incorrect\ngot: %q\nexpected: %q", name, envVar, expected)
	}
}
