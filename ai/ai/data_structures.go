package ai

import (
	"container/heap"
	"fmt"
	"log"
	"math/rand"
	"strconv"
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
	Right
	Down
	Left
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
	InputFoodChannel chan int
	TimeStepChannel  chan time.Duration
	FoodPriority     int
}

type MovementData struct {
	// TilesQueue is a PriorityQueue containing the tiles to visit and loot
	TilesQueue PriorityQueue
	// Path is the path that the player will follow
	Path []RelativeCoordinates
}

type MessagesManagement struct {
	// UUID is the client UUID
	UUID string
	// messageStatusList is a list of all the "useful" status messages
	messageStatusList []broadcastMessageContent
	// Is the player waitingForLevelUp?
	waitingForLevelUp bool
	// Is the player waitingForLevelUpLeech?
	waitingForLevelUpLeech bool
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
	// MessageManager is a struct containing data for managing AI / AI messages
	MessageManager MessagesManagement
	// The number of SlotsLeft
	SlotsLeft int
}

// getInitialDirection fetches the initial direction of the AI from the server
// It ignores any other messages coming before this one
func getInitialDirection(conn network.ServerConn) network.PlayerDirection {
	conn.SendCommand(network.GetDirection, network.EmptyBody)
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

// createUUID creates a new, random UUID for the player
func createUUID(teamName string) string {
	val1 := rand.Int()
	val2 := rand.Int()
	val3 := rand.Int()
	uuidValue := val1 ^ val3 ^ val2
	stringUuidVal := strconv.FormatInt(int64(uuidValue%1000000), 10)
	log.Println("Created player UUID:", stringUuidVal)
	return teamName + stringUuidVal
}

// InitGame creates a new Game struct
func InitGame(serverConn network.ServerConn, teamName string, timeStep, slotsLeft int,
	worldDims RelativeCoordinates) Game {
	initialDirection := network.PlayerDirection(0)
	if worldDims[0] != worldDims[1] {
		initialDirection = getInitialDirection(serverConn)
		if initialDirection == -1 {
			log.Fatal("Failed to get player initial direction")
		}
	}
	game := Game{View: make(ViewMap, 0),
		Inventory: make(Inventory),
		TimeStep:  time.Second / time.Duration(timeStep),
		TeamName:  teamName,
		Socket:    serverConn,
		Coordinates: WorldCoords{CoordsFromOrigin: RelativeCoordinates{0, 0}, Direction: initialDirection,
			WorldSize: worldDims},
		Movement:         MovementData{Path: make([]RelativeCoordinates, 0), TilesQueue: make(PriorityQueue, 0)},
		LevelUpResources: levelUpResources,
		Level:            1,
		MessageManager: MessagesManagement{UUID: createUUID(teamName),
			messageStatusList: make([]broadcastMessageContent, 0)},
		TotalResourcesRequired: totalResourcesRequired,
		SlotsLeft:              slotsLeft,
		FoodManager: FoodManagement{InputFoodChannel: make(chan int),
			TimeStepChannel: make(chan time.Duration), FoodPriority: 1},
	}
	heap.Init(&game.Movement.TilesQueue)
	return game
}

func (game *Game) StartRoutines() {
	go FoodManagementRoutine(game.FoodManager.InputFoodChannel, &game.FoodManager.FoodPriority,
		game.FoodManager.TimeStepChannel)
	go serverResponseRoutine(game.Socket.ResponseFeedbackChannel, game)
	game.FoodManager.TimeStepChannel <- game.TimeStep
}

// EndGame closes the channels when the program exit, thus telling the goroutines to stop
func (game *Game) EndGame() {
	log.Println("Closing channels...")
	close(game.FoodManager.InputFoodChannel)
	close(game.FoodManager.TimeStepChannel)
	close(game.Socket.ResponseFeedbackChannel)
	log.Println("Channels closed")
}

// createInventory creates a new inventory from a parsed double array of strings
func createInventory(parsedInv map[string]int) (Inventory, error) {
	inv := make(Inventory)
	for key, val := range parsedInv {
		if item, ok := parsedKeyToInvKey[key]; ok {
			inv[item] = val
		} else {
			return nil, fmt.Errorf("invalid item in inventory")
		}
	}
	return inv, nil
}
