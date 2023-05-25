package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net"
)

func main() {

	for {
		sendconn, err := net.Dial("tcp", "127.0.0.1:8081")
		if err != nil {
			log.Fatalln("connection", "error", err)
		}
		jsonreq, _ := json.Marshal("AAAA")
		sendconn.Write(jsonreq)
		fmt.Println("TO SEND", "AAAA", "127.0.0.1:8081")
		sendconn.Close()
	}
}
