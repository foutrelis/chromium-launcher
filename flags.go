package main

import (
	"bufio"
	"encoding/json"
	"io/ioutil"
	"os"
	"path/filepath"
	"regexp"

	"github.com/mattn/go-shellwords"
)

func getFlashFlags(flashDirectory string) []string {
	flashVersion := getFlashVersion(flashDirectory)
	if flashVersion == "" {
		return []string{}
	}

	flashPlugin := filepath.Join(flashDirectory, "libpepflashplayer.so")

	return []string{
		"--ppapi-flash-version=" + flashVersion,
		"--ppapi-flash-path=" + flashPlugin}
}

func getFlashVersion(flashDirectory string) string {
	manifest, err := ioutil.ReadFile(filepath.Join(flashDirectory, "manifest.json"))
	if err != nil {
		return ""
	}

	return extractFlashVersion(manifest)
}

func extractFlashVersion(manifest []byte) string {
	var versionInfo flashVersionInfo
	err := json.Unmarshal(manifest, &versionInfo)
	if err != nil {
		return ""
	}

	return versionInfo.Version
}

type flashVersionInfo struct {
	Version string
}

func getUserFlags() []string {
	return readFlags(getUserFlagsFilePath())
}

func readFlags(path string) []string {
	file, err := os.Open(path)
	if err != nil {
		return []string{}
	}
	defer file.Close()

	var flags []string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		newFlags := parseFlags(line)
		for _, flag := range newFlags {
			flags = append(flags, flag)
		}
	}
	return flags
}

func parseFlags(line string) []string {
	isCommentOrEmpty := regexp.MustCompile(`^(\s*#|\s*$)`)

	if isCommentOrEmpty.MatchString(line) {
		return []string{}
	}

	flags, err := shellwords.Parse(line)
	if err != nil {
		return []string{}
	}
	return flags
}

func getUserFlagsFilePath() string {
	return filepath.Join(getConfigHome(), getUserFlagsFileName())
}

func getUserFlagsFileName() string {
	return chromiumName + "-flags.conf"
}

func getConfigHome() string {
	if xdg := os.Getenv("XDG_CONFIG_HOME"); xdg != "" {
		return xdg
	}
	return filepath.Join(os.Getenv("HOME"), ".config")
}
