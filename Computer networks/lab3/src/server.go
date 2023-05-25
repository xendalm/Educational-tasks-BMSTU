package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"net"
	"os"
	"proto"
	"strconv"

	log "github.com/mgutz/logxi/v1"
)

func gcd(a, b int) int {
	for b != 0 {
		a %= b
		a, b = b, a
	}
	return a
}

func main() {
	var (
		serverAddrStr string
		rep           uint
		helpFlag      bool
	)
	flag.StringVar(&serverAddrStr, "addr", "127.0.0.1:6000", "set server IP address and port")
	flag.UintVar(&rep, "rep", 1, "set the number of number of repetitions")
	flag.BoolVar(&helpFlag, "help", false, "print options list")

	if flag.Parse(); helpFlag {
		fmt.Fprint(os.Stderr, "server [options]\n\nAvailable options:\n")
		flag.PrintDefaults()
	} else if serverAddr, err := net.ResolveUDPAddr("udp", serverAddrStr); err != nil {
		log.Error("resolving server address", "error", err)
	} else if conn, err := net.ListenUDP("udp", serverAddr); err != nil {
		log.Error("creating listening connection", "error", err)
	} else {
		log.Info("server listens incoming messages from clients")
		buf := make([]byte, 64)
		var ans []byte
		var previd uint = 0
		for {
			if bytesRead, addr, err := conn.ReadFromUDP(buf); err != nil {
				log.Error("receiving message from client", "error", err)
			} else {
				var args proto.Args
				err := json.Unmarshal(buf[:bytesRead], &args)
				if args.Id != previd {
					previd = args.Id
					ans1 := gcd(args.X, args.Y)
					ans2 := args.X * args.Y / ans1
					ans, _ = json.Marshal(proto.Anss{args.Id, ans1, ans2})

					if err != nil {
						log.Error("cannot parse request", "error", err)
					} else if _, err = conn.WriteToUDP(ans, addr); err != nil {
						log.Error("sending message to client", "error", err, "client", addr.String())
					} else {
						log.Info("successful interaction with client", "ans", strconv.Itoa(ans1)+" "+strconv.Itoa(ans2), "client", addr.String())
					}
					for i := uint(0); i < rep-1; i++ {
						if _, err = conn.WriteToUDP(ans, addr); err != nil {
							log.Error("sending message to client", "error", err, "client", addr.String())
						}
					}
				}
			}
		}
	}
}
