package proto

type Args struct {
	Id uint `json:"id"`
	X  int  `json:"x"`
	Y  int  `json:"y"`
}

type Anss struct {
	Id   uint `json:"id"`
	Ans1 int  `json:"ans1"`
	Ans2 int  `json:"ans2"`
}
