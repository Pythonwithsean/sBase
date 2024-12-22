package main

import "net"

func main() {
	conn, _ := net.Dial("tcp", "localhost:3006")
	conn.Write([]byte("Hello from client"))
}
