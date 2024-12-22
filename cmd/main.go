package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/pythonwithsean/sBase/config"
	"github.com/pythonwithsean/sBase/lib"
	"github.com/pythonwithsean/sBase/server"
)

const VERSION = "v0.1"

func validate(arg string, i int) bool {
	if arg == "" {
		lib.SetLogLevel(lib.WARN)
		lib.WarnLog("Missing argument at position ", i)
		os.Exit(1)
	}
	arg = strings.ToLower(arg)
	switch i {
	case 0:
		return arg == "-h" || arg == "--host"
	case 2:
		return arg == "-p" || arg == "--port"
	case 4:
		return arg == "-u" || arg == "--username"
	case 6:
		return arg == "-pass" || arg == "--password"
	case 8:
		return arg == "-db" || arg == "--database"
	default:
		return false
	}
}

func main() {
	ARGS := os.Args[1:]
	if len(ARGS) != 10 && len(ARGS) != 1 {
		lib.SetLogLevel(lib.WARN)
		lib.WarnLog("Usage: sBase [host] [port] [username] [password] [database]")
		os.Exit(1)
	} else if len(ARGS) == 1 {
		if ARGS[0] == "-v" || ARGS[0] == "--version" {
			fmt.Println("sBase version:", VERSION)
			os.Exit(0)
		} else if ARGS[0] == "-h" || ARGS[0] == "--help" {
			fmt.Println("Welcome to sBase Help Page")
			os.Exit(0)
		} else {
			lib.SetLogLevel(lib.WARN)
			lib.WarnLog("Usage: sBase [host] [port] [username] [password] [database]")
			os.Exit(1)
		}
	}

	cmds := map[string]string{}
	for i := 0; i < len(ARGS); i += 2 {
		if ok := validate(ARGS[i], i); ok {
			if ARGS[i+1] != "" {
				cmds[ARGS[i]] = ARGS[i+1]
			} else {
				lib.SetLogLevel(lib.WARN)
				lib.WarnLog("Missing argument for ", ARGS[i])
				os.Exit(1)
			}
		} else {
			lib.SetLogLevel(lib.WARN)
			lib.WarnLog("Invalid argument ", ARGS[i])
			os.Exit(1)
		}
	}

	db_config := config.DB_CONFIG{}
	db_config.SetDBName(cmds["-db"]).SetPassword(cmds["-pass"]).SetPort(cmds["-p"]).SetUsername(cmds["-u"]).SetHostName(cmds["-h"])
	tcpServer := server.NewTCPServer(cmds["-h"], cmds["-p"])
	tcpServer.Start()
	// ui.Index()
}
