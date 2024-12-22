package lib

import (
	"log"
	"os"
)

type LogLevel int

var (
	info_logger  = log.New(os.Stdout, "\033[34m [INFO]: ", log.Ldate|log.Ltime)
	error_logger = log.New(os.Stderr, "\033[31m [ERROR]: ", log.Ldate|log.Ltime)
	warn_logger  = log.New(os.Stdout, "\033[33m [WARN]: ", log.Ldate|log.Ltime)
	debug_logger = log.New(os.Stdout, "\033[36m [DEBUG]: ", log.Ldate|log.Ltime)
)

const (
	DEBUG LogLevel = iota
	INFO
	WARN
	ERROR
)

var log_level LogLevel = 0

func SetLogLevel(level LogLevel) {
	log_level = level
}

func InfoLog(message ...any) {
	if log_level >= INFO {
		info_logger.Println(message...)
	}
}

func ErrorLog(err ...any) {
	if log_level >= ERROR {
		error_logger.Println(err...)
	}
}

func WarnLog(message ...any) {
	if log_level >= WARN {
		warn_logger.Println(message...)
	}
}

func DebugLog(message ...any) {
	if log_level >= DEBUG {
		debug_logger.Println(message...)
	}
}
