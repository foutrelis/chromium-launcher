package main

import (
	"os"
	"path/filepath"
	"reflect"
	"testing"
)

func TestGetFlashFlags(t *testing.T) {
	runGetFlashFlagsTestCase(t, "testdata", []string{
		"--ppapi-flash-version=4.5.6",
		"--ppapi-flash-path=testdata/libpepflashplayer.so"})
	runGetFlashFlagsTestCase(t, "fakedata", []string{})
}

func runGetFlashFlagsTestCase(t *testing.T, path string, expected []string) {
	flags := getFlashFlags(path)
	if ! reflect.DeepEqual(flags, expected) {
		t.Errorf("Reading flash flags from %s failed\ngot: %#v\nexpected: %#v", path, flags, expected)
	}
}

func TestGetFlashVersion(t *testing.T) {
	if flashVersion := getFlashVersion("testdata"); flashVersion != "4.5.6" {
		t.Errorf("flashVersion: %q, expected: \"4.5.6\"", flashVersion)
	}

	if flashVersion := getFlashVersion("fakedata"); flashVersion != "" {
		t.Errorf("flashVersion: %q, expected: \"\"", flashVersion)
	}
}

func TestExtractFlashVersion(t *testing.T) {
	manifest := []byte(`{"version": "1.2.3"}`)

	if flashVersion := ExtractFlashVersion(manifest); flashVersion != "1.2.3" {
		t.Errorf("flashVersion: %q, expected: \"1.2.3\"", flashVersion)
	}

	if flashVersion := ExtractFlashVersion(manifest[1:]); flashVersion != "" {
		t.Errorf("flashVersion: %q, expected: \"\"", flashVersion)
	}
}

func TestReadFlags(t *testing.T) {
	runReadFlagsTestCase(t, "testdata/chromium-flags.conf",
		[]string{"--if", "--it", "--builds", "--it --ships"})
	runReadFlagsTestCase(t, "fakedata/chromium-flags.conf",
		[]string{})
}

func runReadFlagsTestCase(t *testing.T, path string, expected []string) {
	flags := readFlags(path)
	if ! reflect.DeepEqual(flags, expected) {
		t.Errorf("Reading flags from %s failed\ngot: %#v\nexpected: %#v", path, flags, expected)
	}
}
func TestParseFlags(t *testing.T) {
	runParseFlagsTestCase(t, "comment line", "# comment", []string{})
	runParseFlagsTestCase(t, "whitespace-only line", "  \t", []string{})
	runParseFlagsTestCase(t, "flags with quotes",
		`--double="a b c" --single='a b c'`,
		[]string{"--double=a b c", "--single=a b c"})
	runParseFlagsTestCase(t, "unbalanced quotes", `'--flag-a "--flag-b"`, []string{})
}

func runParseFlagsTestCase(t *testing.T, desc string, line string, expected []string) {
	flags := ParseFlags(line)
	if ! reflect.DeepEqual(flags, expected) {
		t.Errorf("Parsing %s failed\ngot: %#v\nexpected: %#v", desc, flags, expected)
	}
}

func TestGetConfigHome(t *testing.T) {
	oldConfigHome := os.Getenv("XDG_CONFIG_HOME")
	defer func() {
		os.Setenv("XDG_CONFIG_HOME", oldConfigHome)
	}()

	os.Setenv("XDG_CONFIG_HOME", "test-xdg-config-home")
	if configHome := getConfigHome(); configHome != "test-xdg-config-home" {
		t.Errorf("getConfigHome() returned %q instead of %q", configHome, "test-xdg-config-home")
	}

	os.Unsetenv("XDG_CONFIG_HOME")
	if configHome := getConfigHome(); configHome != filepath.Join(os.Getenv("HOME"), ".config") {
		t.Errorf("getConfigHome() returned %q instead of %q", configHome, `$HOME/.config`)
	}
}
