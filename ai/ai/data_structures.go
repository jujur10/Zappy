package ai

import (
	"container/heap"
	"fmt"
	"strings"
	"zappy_ai/network"
)

type TileItem int

const (
	// A Player on the tile
	Player TileItem = iota
	// Food on the tile
	Food
	// Linemate on the tile
	Linemate
	// Deraumere on the tile
	Deraumere
	// Sibur on the tile
	Sibur
	// Mendiane on the tile
	Mendiane
	// Phiras on the tile
	Phiras
	// Thystame on the tile
	Thystame
)

// A ViewMap is a double array of TileItem
type ViewMap [][]TileItem

// An Inventory is just a map of element, indexed via TileItem
type Inventory map[TileItem]int

// RelativeCoordinates from the origin / spawn point
type RelativeCoordinates [2]int

// PlayerDirection The Direction in which the Player is going
type PlayerDirection int

const (
	Up PlayerDirection = iota
	Left
	Down
	Right
)

// The WorldCoords of the player
type WorldCoords struct {
	// The CoordsFromOrigin, modulo map dimensions
	CoordsFromOrigin RelativeCoordinates
	// The Direction in which the player is going
	Direction PlayerDirection
}

// The main struct containing the Game data
type Game struct {
	// The player View
	View ViewMap
	// The player Inventory
	Inventory Inventory
	// The TimeStep of the game
	TimeStep int
	// TeamName is the name of the player's team
	TeamName string
	// The server Socket
	Socket network.ServerConn
	// The world Coordinates of the player
	Coordinates WorldCoords
	// MovementQueue is a PriorityQueue containing the future movements of the player
	MovementQueue PriorityQueue
	// The Level of the player
	Level int
	// LevelUpResources is a map of TileItem -> Inventory containing the necessary resources for level ups
	LevelUpResources map[int]Inventory
	// TotalResourcesRequired is an Inventory containing the total resources left to collect to reach level 8
	TotalResourcesRequired Inventory
	//
	FoodChannel chan int
}

// InitGame creates a new Game struct
func InitGame(serverConn network.ServerConn, teamName string, timeStep int) Game {
	game := Game{View: make(ViewMap, 0),
		Inventory:              make(Inventory),
		TimeStep:               timeStep,
		TeamName:               teamName,
		Socket:                 serverConn,
		Coordinates:            WorldCoords{CoordsFromOrigin: RelativeCoordinates{0, 0}, Direction: 0},
		MovementQueue:          make(PriorityQueue, 10),
		LevelUpResources:       levelUpResources,
		TotalResourcesRequired: totalResourcesRequired,
	}
	heap.Init(&game.MovementQueue)
	return game
}

// CreateViewMap creates a ViewMap from a parsed double array of strings
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
