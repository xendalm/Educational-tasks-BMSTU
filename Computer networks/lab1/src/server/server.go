package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"math"
	"net"
	"proto"
	"strconv"

	log "github.com/mgutz/logxi/v1"
)

// Client - состояние клиента.
type Client struct {
	logger   log.Logger    // Объект для печати логов
	conn     *net.TCPConn  // Объект TCP-соединения
	enc      *json.Encoder // Объект для кодирования и отправки сообщений
	ans      proto.Answer
	segments bool
}

// NewClient - конструктор клиента, принимает в качестве параметра
// объект TCP-соединения.
func NewClient(conn *net.TCPConn) *Client {
	return &Client{
		logger:   log.New(fmt.Sprintf("client %s", conn.RemoteAddr().String())),
		conn:     conn,
		enc:      json.NewEncoder(conn),
		ans:      proto.Answer{"0", proto.Point{"0", "0"}},
		segments: false,
	}
}

// serve - метод, в котором реализован цикл взаимодействия с клиентом.
// Подразумевается, что метод serve будет вызаваться в отдельной go-программе.
func (client *Client) serve() {
	defer client.conn.Close()
	decoder := json.NewDecoder(client.conn)
	for {
		var req proto.Request
		if err := decoder.Decode(&req); err != nil {
			client.logger.Error("cannot decode message", "reason", err)
			break
		} else {
			client.logger.Info("received command", "command", req.Command)
			if client.handleRequest(&req) {
				client.logger.Info("shutting down connection")
				break
			}
		}
	}
}

// handleRequest - метод обработки запроса от клиента. Он возвращает true,
// если клиент передал команду "quit" и хочет завершить общение.
func (client *Client) handleRequest(req *proto.Request) bool {
	switch req.Command {
	case "quit":
		client.respond("ok", nil)
		return true
	case "new":
		errorMsg := ""
		if req.Data == nil {
			errorMsg = "data field is absent"
		} else {
			var points [4]proto.Point
			if err := json.Unmarshal(*req.Data, &points); err != nil {
				errorMsg = "malformed data field"
			} else {
				var ptArray [4]pt
				breakFlag := false
				for i := 0; i < 4; i++ {
					x, err1 := strconv.ParseFloat(points[i].X, 64)
					y, err2 := strconv.ParseFloat(points[i].Y, 64)
					if err1 != nil || err2 != nil {
						errorMsg = "malformed data field"
						breakFlag = true
						break
					}
					ptArray[i] = pt{x, y}
				}
				if !breakFlag {
					cond, intersection_point := intersect(ptArray)
					client.ans.Cond = cond
					client.ans.P.X = fmt.Sprintf("%f", intersection_point.x)
					client.ans.P.Y = fmt.Sprintf("%f", intersection_point.y)
					client.segments = true
				}
			}
		}
		if errorMsg == "" {
			client.respond("ok", nil)
		} else {
			client.logger.Error("addition failed", "reason", errorMsg)
			client.respond("failed", errorMsg)
		}
	case "get":
		if !client.segments {
			client.logger.Error("calculation failed", "reason", "no segments")
			client.respond("failed", "no segments")
		} else {
			client.respond("result", &proto.Answer{
				Cond: client.ans.Cond,
				P:    client.ans.P,
			})
		}
	default:
		client.logger.Error("unknown command")
		client.respond("failed", "unknown command")
	}
	return false
}

//нахождение точки пересечения
type pt struct {
	x, y float64
}

var (
	EPS float64 = 1e-9
)

func det(a, b, c, d float64) float64 {
	return a*d - b*c
}

func between(a, b, c float64) bool {
	return math.Min(a, b) <= c+EPS && c <= math.Max(a, b)+EPS
}

func intersect_1(a, b, c, d float64) bool {
	if a > b {
		a, b = b, a
	}
	if c > d {
		c, d = d, c
	}
	return math.Max(a, c) <= math.Min(b, d)
}

func intersect(points [4]pt) (string, pt) {
	var A1, B1 float64 = points[0].y - points[1].y, points[1].x - points[0].x
	C1 := -A1*points[0].x - B1*points[0].y
	var A2, B2 = points[2].y - points[3].y, points[3].x - points[2].x
	C2 := -A2*points[2].x - B2*points[2].y
	zn := det(A1, B1, A2, B2)
	if zn != 0 {
		var x, y float64 = -det(C1, B1, C2, B2) / zn, -det(A1, C1, A2, C2) / zn
		f := between(points[0].x, points[1].x, x) && between(points[0].y, points[1].y, y) && between(points[2].x, points[3].x, x) && between(points[2].y, points[3].y, y)
		cond := "0"
		if f {
			cond = "1"
		}
		return cond, pt{x, y}
	} else {
		f := det(A1, C1, A2, C2) == 0 && det(B1, C1, B2, C2) == 0 && intersect_1(points[0].x, points[1].x, points[2].x, points[3].x) && intersect_1(points[0].y, points[1].y, points[2].y, points[3].y)
		cond := "0"
		if f {
			cond = "2"
		}
		return cond, pt{0, 0}
	}
}

// respond - вспомогательный метод для передачи ответа с указанным статусом
// и данными. Данные могут быть пустыми (data == nil).
func (client *Client) respond(status string, data interface{}) {
	var raw json.RawMessage
	raw, _ = json.Marshal(data)
	client.enc.Encode(&proto.Response{status, &raw})
}

func main() {
	// Работа с командной строкой, в которой может указываться необязательный ключ -addr.
	var addrStr string
	flag.StringVar(&addrStr, "addr", "127.0.0.1:9000", "specify ip address and port")
	flag.Parse()

	// Разбор адреса, строковое представление которого находится в переменной addrStr.
	if addr, err := net.ResolveTCPAddr("tcp", addrStr); err != nil {
		log.Error("address resolution failed", "address", addrStr)
	} else {
		log.Info("resolved TCP address", "address", addr.String())

		// Инициация слушания сети на заданном адресе.
		if listener, err := net.ListenTCP("tcp", addr); err != nil {
			log.Error("listening failed", "reason", err)
		} else {
			// Цикл приёма входящих соединений.
			for {
				if conn, err := listener.AcceptTCP(); err != nil {
					log.Error("cannot accept connection", "reason", err)
				} else {
					log.Info("accepted connection", "address", conn.RemoteAddr().String())

					// Запуск go-программы для обслуживания клиентов.
					go NewClient(conn).serve()
				}
			}
		}
	}
}
