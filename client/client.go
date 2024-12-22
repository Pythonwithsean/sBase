package main

import (
	"net"

	"github.com/pythonwithsean/sBase/lib"
)

func main() {
	conn, err := net.Dial("tcp", "localhost:3006")
	if err != nil {
		lib.SetLogLevel(lib.ERROR)
		lib.ErrorLog("Error connecting to server")
	}
	conn.Write([]byte("Hello from client"))
}
