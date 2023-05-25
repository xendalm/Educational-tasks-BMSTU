package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net"
)

func serverfunc(ln net.Listener) {
	// Открываем порт
	conn, _ := ln.Accept()
	defer conn.Close()

	decoder := json.NewDecoder(conn)
	// Запускаем цикл
	for {
		var req Request
		err := decoder.Decode(&req)
		if err != nil {
			log.Fatalln(err)
			return
		} else {
			if err != nil {
				log.Fatalln("cannot parse request", err)
			}
			switch req.Command {
			case "add":
				addresses[req.Name] = req.Address
			case "del":
				delete(addresses, req.Name)
			case "openconn":
				tempconn, err := net.Dial("tcp", nextAddrStr)
				sendconn = tempconn
				f = true
				if err != nil {
					log.Fatalln(err)
				}
				fmt.Print("\n!!!OPENCONN!!!\nCommand:")
			}
			if req.AddrStr != nextAddrStr {
				jsonreq, _ := json.Marshal(req)
				sendconn.Write(jsonreq)
			}
			//conn.Write([]byte("GOT IT" + "\n"))
			if req.Command == "openconn" {
				for _, v := range reqBuffer {
					jsonreq, _ := json.Marshal(v)
					sendconn.Write(jsonreq)
				}
				reqBuffer = make([]Request, 0)
			}
		}
	}
}

type Request struct {
	Command string
	Name    string
	Address string
	AddrStr string
}

var (
	addresses   map[string]string
	myAddrStr   string
	nextAddrStr string
	f           bool = false
	sendconn    net.Conn
	reqBuffer   []Request
)

func main() {
	flag.StringVar(&myAddrStr, "addr", "127.0.0.1:6000", "set client IP address and port")
	flag.StringVar(&nextAddrStr, "naddr", "127.0.0.1:6000", "set client IP address and port")
	flag.Parse()
	addresses = make(map[string]string, 100)
	var command, name, address string
	// Устанавливаем прослушивание порта
	ln, _ := net.Listen("tcp", myAddrStr)
	tempconn, err := net.Dial("tcp", nextAddrStr)
	if err == nil {
		sendconn = tempconn
		f = true
		jsonreq, _ := json.Marshal(Request{"openconn", "", "", myAddrStr})
		sendconn.Write(jsonreq)
		fmt.Println("!!!OPENCONN!!!")
	}
	go serverfunc(ln)
	for {
		if f {
			if command == "add" || command == "del" {
				jsonreq, _ := json.Marshal(Request{command, name, address, myAddrStr})
				sendconn.Write(jsonreq)
			}
			for {
				fmt.Print("Command: ")
				fmt.Scan(&command)
				switch command {
				case "add":
					fmt.Scan(&name, &address)
					addresses[name] = address
					// Отправляем в socket
					jsonreq, _ := json.Marshal(Request{command, name, address, myAddrStr})
					sendconn.Write(jsonreq)
					// Прослушиваем ответ
					// message, _ := bufio.NewReader(sendconn).ReadString('\n')
					// fmt.Print("Message from server: " + message)
				case "del":
					fmt.Scan(&name)
					delete(addresses, name)
					// Отправляем в socket
					jsonreq, _ := json.Marshal(Request{command, name, address, myAddrStr})
					sendconn.Write(jsonreq)
					// Прослушиваем ответ
					// message, _ := bufio.NewReader(sendconn).ReadString('\n')
					// fmt.Print("Message from server: " + message)
				case "show":
					log.Println("show - result: ")
					i := 1
					for k, v := range addresses {
						fmt.Printf("\t%d) %s - %s\n", i, k, v)
						i++
					}
				case "get":
					fmt.Scan(&name)
					v, ok := addresses[name]
					if ok {
						log.Println("get "+name+" - result: ", v)
					} else {
						log.Println("get "+name+" - result: ", "no address")
					}
				default:
					log.Println("invalid command")
				}
			}
		}
		fmt.Print("Command: ")
		fmt.Scan(&command)
		switch command {
		case "add":
			fmt.Scan(&name, &address)
			addresses[name] = address
			reqBuffer = append(reqBuffer, Request{command, name, address, myAddrStr})
		case "del":
			fmt.Scan(&name)
			delete(addresses, name)
			reqBuffer = append(reqBuffer, Request{command, name, address, myAddrStr})
		case "show":
			log.Println("show - result: ")
			i := 1
			for k, v := range addresses {
				fmt.Printf("\t%d) %s - %s\n", i, k, v)
				i++
			}
		case "get":
			fmt.Scan(&name)
			v, ok := addresses[name]
			if ok {
				log.Println("get "+name+" - result: ", v)
			} else {
				log.Println("get "+name+" - result: ", "no address")
			}
		default:
			log.Println("invalid command")
		}
	}
}
