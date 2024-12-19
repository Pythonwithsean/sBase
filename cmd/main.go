package main

import (
	"os"

	"github.com/pythonwithsean/sBase/lib"
)

const VERSION = "0.1-Dev"

func main() {
	if len(os.Args) < 4 {
		lib.SetLogLevel(lib.WARN)
		lib.WarnLog("Usage: sBase [host] [port] [username] [password] [database]")
		os.Exit(1)
	}

}
