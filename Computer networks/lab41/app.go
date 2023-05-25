package main

import (
	"log"
	"net/http"
	"os/exec"
	"path/filepath"
	"strings"
	"text/template"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

func HomeRouterHandler(w http.ResponseWriter, r *http.Request) {
	path := filepath.Join("page.html")
	tmpl, err := template.ParseFiles(path)
	if err != nil {
		http.Error(w, err.Error(), 400)
		return
	}

	cmd := exec.Command("chcp", "65001")
	if err := cmd.Run(); err != nil {
		log.Fatal(err)
	}

	err = tmpl.Execute(w, nil)
	if err != nil {
		http.Error(w, err.Error(), 400)
		return
	}
}

func reader(conn *websocket.Conn) {
	for {
		messageType, message, err := conn.ReadMessage()
		if err != nil {
			log.Println("Error during message reading:", err)
			break
		}
		log.Printf("Received: %s", message)
		if len(message) != 0 {
			args := strings.Fields(string(message))
			result, _ := exec.Command(args[1], args[0]).Output()
			msg := string(result)

			if err := conn.WriteMessage(messageType, []byte(msg)); err != nil {
				log.Println(err)
				return
			}
		} else {
			err = conn.WriteMessage(messageType, message)
			if err != nil {
				log.Println("Error during message writing:", err)
				break
			}
		}
	}
}

func wsEndpoint(w http.ResponseWriter, r *http.Request) {
	upgrader.CheckOrigin = func(r *http.Request) bool { return true }

	wsconn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println(err)
	}

	log.Println("Connected")

	go reader(wsconn)
}

func main() {
	http.HandleFunc("/", HomeRouterHandler)
	http.HandleFunc("/ws", wsEndpoint)
	err := http.ListenAndServe(":8080", nil)
	if err != nil {
		log.Fatal(err)
	}
}
