package ai

import (
	"container/heap"
	"fmt"
	"log"
	"math/rand"
	"strconv"
	"strings"
	"time"
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

const (
	Up network.PlayerDirection = iota
	Left
	Down
	Right
)

// A ViewMap is a double array of TileItem
type ViewMap [][]TileItem

// An Inventory is just a map of element, indexed via TileItem
type Inventory map[TileItem]int

// RelativeCoordinates from the origin / spawn point
type RelativeCoordinates [2]int

// The WorldCoords of the player
type WorldCoords struct {
	// The CoordsFromOrigin, modulo map dimensions
	CoordsFromOrigin RelativeCoordinates
	// The Direction in which the player is going
	Direction network.PlayerDirection
	// The WorldSize
	WorldSize RelativeCoordinates
}

// FoodManagement is a struct containing values for managing food
type FoodManagement struct {
	// Channel to communicate with the food management goroutine.
	// New food comes in, and food priority comes out
	// In case of death by starvation, priority is -1
	FoodChannel  chan int
	FoodPriority int
}

type MovementData struct {
	// TilesQueue is a PriorityQueue containing the tiles to visit and loot
	TilesQueue PriorityQueue
	// Path is the path that the player will follow
	Path []RelativeCoordinates
}

// Game is the main struct containing the game data
type Game struct {
	// The player View
	View ViewMap
	// The player Inventory
	Inventory Inventory
	// The TimeStep of the game
	TimeStep time.Duration
	// TeamName is the name of the player's team
	TeamName string
	// The server Socket
	Socket network.ServerConn
	// The world Coordinates of the player
	Coordinates WorldCoords
	// Movement contains data for the player's movement
	Movement MovementData
	// The Level of the player
	Level int
	// LevelUpResources is a map of TileItem -> Inventory containing the necessary resources for level ups
	LevelUpResources map[int]Inventory
	// TotalResourcesRequired is an Inventory containing the total resources left to collect to reach level 8
	TotalResourcesRequired Inventory
	// FoodManager is a struct containing values for managing food
	FoodManager FoodManagement
	// UUID is the client UUID
	UUID string
}

// getInitialDirection fetches the initial direction of the AI from the server
// It ignores any other messages coming before this one
func getInitialDirection(conn network.ServerConn) network.PlayerDirection {
	conn.GetDirection()
	respType := network.Nil
	for respType != network.Direction {
		rType, value, err := conn.GetAndParseResponse()
		if rType == network.Direction && err == nil {
			return value.(network.PlayerDirection)
		}
		respType = rType
	}
	return -1
}

func createUUID(teamName string) string {
	val1 := rand.Int()
	val2 := rand.Int()
	val3 := rand.Int()
	uuidValue := val1 ^ val3 ^ val2
	stringUuidVal := strconv.FormatInt(int64(uuidValue%1000000), 10)
	return teamName + stringUuidVal
}

// InitGame creates a new Game struct
func InitGame(serverConn network.ServerConn, teamName string, timeStep int) Game {
	initialDirection := getInitialDirection(serverConn)
	if initialDirection == -1 {
		log.Fatal("Failed to get player initial direction")
	}
	game := Game{View: make(ViewMap, 0),
		Inventory:              make(Inventory),
		TimeStep:               time.Second / time.Duration(timeStep),
		TeamName:               teamName,
		Socket:                 serverConn,
		Coordinates:            WorldCoords{CoordsFromOrigin: RelativeCoordinates{0, 0}, Direction: initialDirection},
		Movement:               MovementData{Path: make([]RelativeCoordinates, 0), TilesQueue: make(PriorityQueue, 0)},
		LevelUpResources:       levelUpResources,
		Level:                  1,
		UUID:                   createUUID(teamName),
		TotalResourcesRequired: totalResourcesRequired,
		FoodManager:            FoodManagement{FoodChannel: make(chan int), FoodPriority: 1},
	}
	heap.Init(&game.Movement.TilesQueue)
	go FoodManagementRoutine(game.FoodManager.FoodChannel, game.TimeStep)
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
