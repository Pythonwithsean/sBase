package main

import (
	"flag"
	"fmt"
	"os"
	"path"

	"github.com/tobshub/tobsdb/internal/builder"
)

const VERSION = "1.1.7-dev"

func main() {
	db_write_path := flag.String("db", "", "path to load and save db data")
	in_mem := flag.Bool("m", false, "use in-memory mode: don't persist db")
	port := flag.Int("port", 7085, "listening port")
	should_log := flag.Bool("log", false, "print error logs")
	show_debug_logs := flag.Bool("dbg", false, "show extra logs")
	username := flag.String("u", os.Getenv("TDB_USER"), "username")
	password := flag.String("p", os.Getenv("TDB_PASS"), "password")
	idle_interval := flag.Int("w", 1000, "time to wait before writing data when idle")
	print_version := flag.Bool("v", false, "print version and exit")

	flag.Parse()

	if len(*db_write_path) > 0 && !path.IsAbs(*db_write_path) {
		cwd, _ := os.Getwd()
		*db_write_path = path.Join(cwd, *db_write_path)
	}

	if *print_version {
		fmt.Printf("TobsDB Server v%s\n", VERSION)
		os.Exit(0)
	}

	os.Setenv("TDB_USER", *username)
	os.Setenv("TDB_PASS", *password)

	log_options := builder.LogOptions{
		Should_log:      *should_log,
		Show_debug_logs: *show_debug_logs,
	}
	write_settings := builder.NewWriteSettings(*db_write_path, *in_mem, *idle_interval)

	db := builder.NewTobsDB(write_settings, log_options)
	db.Listen(*port)
}
