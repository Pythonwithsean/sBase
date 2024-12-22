package server

import (
	"fmt"
	"log"
	"net"
)

type TCPServer struct {
	host string
	port string
}

func NewTCPServer(host, port string) *TCPServer {
	return &TCPServer{
		host: host,
		port: port,
	}
}

func (s *TCPServer) Start() {
	// Start the TCP server
	fmt.Println("Starting TCP server on", s.host+":"+s.port)
	listener, err := net.Listen("tcp", s.host+":"+s.port)
	if err != nil {
		fmt.Println("Error starting the server")
		log.Fatal(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Error accepting connection")
			log.Fatal(err)
		}
		fmt.Println("Accepted connection from", conn.RemoteAddr())
		payload := make([]byte, 1024)
		n, _ := conn.Read(payload)
		fmt.Println("Received message:", string(payload[:n]))

	}

	defer listener.Close()
}
