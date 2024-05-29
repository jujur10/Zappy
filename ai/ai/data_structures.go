package ai

import (
	"fmt"
	"strings"
	"zappy_ai/network"
)

type TileItem int

const (
	Player TileItem = iota
	Food
	Linemate
	Deraumere
	Sibur
	Mendiane
	Phiras
	Thystame
)

type ViewMap [][]TileItem
type Inventory map[string]int

type Game struct {
	View      ViewMap
	Inventory Inventory
	TimeStep  int
	TeamName  string
	Socket    network.ServerConn
}

func CreateViewMap(parsedList [][]string) (ViewMap, error) {
	size := 0
	counter := 1
	for len(parsedList) > size {
		size += counter
		counter += 2
	}
	if len(parsedList) != size {
		return nil, fmt.Errorf("invalid number of items in view map")
	}
	viewMap := make(ViewMap, len(parsedList))
	for idx, tile := range parsedList {
		viewMap[idx] = make([]TileItem, len(tile))
		for itemIdx, tileItem := range tile {
			tileItem = strings.ToLower(tileItem)
			var item TileItem
			switch tileItem {
			case "player":
				item = Player
			case "food":
				item = Food
			case "linemate":
				item = Linemate
			case "deraumere":
				item = Deraumere
			case "sibur":
				item = Sibur
			case "mendiane":
				item = Mendiane
			case "phiras":
				item = Phiras
			case "thystame":
				item = Thystame
			default:
				return nil, fmt.Errorf("invalid tileItem: %s", tileItem)
			}
			viewMap[idx][itemIdx] = item
		}
	}
	return viewMap, nil
}
