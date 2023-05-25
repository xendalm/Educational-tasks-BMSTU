package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"math/rand"
	"net"
	"os"
	"proto"
	"strconv"
	"time"

	log "github.com/mgutz/logxi/v1"
)

func main() {
	var (
		serverAddrStr string
		n             uint
		rep           uint
		helpFlag      bool
	)
	flag.StringVar(&serverAddrStr, "server", "127.0.0.1:6000", "set server IP address and port")
	flag.UintVar(&n, "n", 10, "set the number of requests")
	flag.UintVar(&rep, "rep", 1, "set the number of number of repetitions")
	flag.BoolVar(&helpFlag, "help", false, "print options list")

	if flag.Parse(); helpFlag {
		fmt.Fprint(os.Stderr, "client [options]\n\nAvailable options:\n")
		flag.PrintDefaults()
	} else if serverAddr, err := net.ResolveUDPAddr("udp", serverAddrStr); err != nil {
		log.Error("resolving server address", "error", err)
	} else if conn, err := net.DialUDP("udp", nil, serverAddr); err != nil {
		log.Error("creating connection to server", "error", err)
	} else {
		defer conn.Close()

		rand.Seed(time.Now().UnixNano())
		conn.SetReadDeadline(time.Now().Add(10 * time.Second))
		buf := make([]byte, 64)
		for i := uint(0); i < n; i++ {
			x, y := rand.Intn(1000), rand.Intn(1000)
			id := uint(rand.Intn(10000))
			req, _ := json.Marshal(proto.Args{id, x, y})

			writeflag := true
			for i := uint(0); i < rep; i++ {
				if _, err := conn.Write(req); err != nil {
					log.Error("sending request to server", "error", err, "x", x)
					writeflag = false
				}
			}
			var anss proto.Anss
			readid := uint(0)
			readflag := true
			for readid != id {
				if bytesRead, err := conn.Read(buf); err != nil && writeflag {
					log.Error("receiving answer from server", "error", err)
					readflag = false
					break
				} else {
					err := json.Unmarshal(buf[:bytesRead], &anss)
					if err != nil {
						log.Error("cannot parse answer", "error", err)
						readflag = false
						break
					} else {
						readid = anss.Id
					}
				}
			}
			if readflag {
				log.Info("successful interaction with server", "req", strconv.Itoa(x)+" "+strconv.Itoa(y), "ans1", anss.Ans1, "ans2", anss.Ans2)
			}
		}
	}
}
