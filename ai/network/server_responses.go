package network

import (
	"fmt"
	"strconv"
	"strings"
)

type MessageType int
type EventDirection int

type BroadcastData struct {
	// The direction from which the message is comming, (from 1 to 8, 0 is the current tile)
	direction EventDirection
	// The text of the message
	text string
}

const (
	// A Boolean type response
	Boolean MessageType = iota
	// An Inventory
	Inventory
	// A player View list
	View
	// An Int type response
	Int
	// A Broadcast message
	Broadcast
	// A Direction
	Direction
	// An Elevation message
	Elevation
	// A message signifying the Death of a player
	Death
	// Nil means an error / nothing
	Nil
)

var inventoryIndexes = []string{"food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"}
var validObjects = []string{"food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame", "player"}

var validResponsesTypes = map[CommandType][]MessageType{
	RotateRight:    {Boolean},
	RotateLeft:     {Boolean},
	GoForward:      {Boolean},
	LookAround:     {View},
	GetInventory:   {Inventory},
	BroadcastText:  {Boolean},
	GetUnusedSlots: {Int},
	Fork:           {Boolean},
	EjectPlayers:   {Boolean},
	TakeObject:     {Boolean},
	SetObject:      {Boolean},
	LevelUp:        {Elevation, Boolean},
	None:           {},
}

// getServerResponse reads a line from the server socket, or returns an error if a line is not available
func (conn ServerConn) getServerResponse() (string, error) {
	line, err := conn.Reader.ReadLine()
	if err != nil {
		return "", err
	}
	return line, nil
}

// IsInArray T is a type template constrained by 'comparable'
func IsInArray[T comparable](value T, array []T) bool {
	for _, v := range array {
		if v == value {
			return true
		}
	}
	return false
}

// checkValues verifies the validity of the view (length, player at current pos and all elems are valid)
func checkValues(values [][]string) bool {
	if !IsInArray("player", values[0]) {
		return false
	}
	size := 0
	counter := 1
	for len(values) > size {
		size += counter
		counter += 2
	}
	if len(values) != size {
		return false
	}
	for _, tile := range values {
		for _, val := range tile {
			if !IsInArray(val, validObjects) {
				return false
			}
		}
	}
	return true
}

// parseInventory parses the inventory content and returns it, or an error if the parsing failed
func parseInventory(values [][]string) (any, error) {
	if len(values) != len(inventoryIndexes) {
		return nil, fmt.Errorf("invalid number of inventory indexes: %d", len(values))
	}
	inventory := make(map[string]int)

	for _, pair := range values {
		if len(pair) != 2 {
			return nil, fmt.Errorf("invalid inventory format")
		}
		val, err := strconv.Atoi(pair[1])
		if !IsInArray(pair[0], inventoryIndexes) || err != nil {
			return nil, fmt.Errorf("invalid inventory format")
		}
		inventory[pair[0]] = val
	}
	return inventory, nil
}

// parseArray parses an array sent by the server and returns its parsed form or an error on failure
func parseArray(line string) (MessageType, any, error) {
	line = strings.Trim(line, "[]")
	values := strings.Split(line, ",")
	individualValues := make([][]string, len(values))
	for idx, value := range values {
		values[idx] = strings.TrimSpace(value)
		individualValues[idx] = strings.Split(values[idx], " ")
		if values[idx] == "" {
			individualValues[idx] = []string{}
		}
	}

	if len(individualValues) == len(inventoryIndexes) && len(individualValues[0]) == 2 {
		inventory, inventoryErr := parseInventory(individualValues)
		if inventoryErr == nil {
			return Inventory, inventory, nil
		}
	}

	if checkValues(individualValues) == false {
		return Nil, nil, fmt.Errorf("Invalid values\n")
	}

	return View, individualValues, nil
}

// parseUnexpectedMessage parses a message which is NOT a response to a client command, then returns it or an error
func parseUnexpectedMessage(line string) (MessageType, any, error) {
	if line == "dead" {
		return Death, nil, nil
	}
	if strings.HasPrefix(line, "eject: ") {
		line = strings.TrimPrefix(line, "eject: ")
		val, err := strconv.Atoi(line)
		if err == nil && val >= 0 && val < 9 {
			return Direction, EventDirection(val), nil
		}
	}
	if strings.HasPrefix(line, "message ") {
		line = strings.TrimPrefix(line, "message ")
		rest := strings.SplitN(line, ",", 2)
		rest[1] = strings.TrimSpace(rest[1])
		val, err := strconv.Atoi(rest[0])
		if err == nil && val >= 0 && val < 9 {
			return Broadcast, BroadcastData{direction: EventDirection(val), text: rest[1]}, nil
		}
	}
	return Nil, nil, fmt.Errorf("Invalid command\n")
}

// GetAndParseResponse reads a response from the server connection and parses it
func (conn ServerConn) GetAndParseResponse() (MessageType, any, error) {
	line, err := conn.getServerResponse()
	if err != nil {
		return Nil, nil, err
	}
	if line == "ok" {
		return Boolean, true, nil
	}
	if line == "ko" {
		return Boolean, false, nil
	}
	if line[0] == '[' && line[len(line)-1] == ']' {
		return parseArray(line)
	}
	val, err := strconv.Atoi(line)
	if err == nil {
		return Int, val, nil
	}
	unexpectedType, unexpectedValue, err := parseUnexpectedMessage(line)
	if err == nil {
		return unexpectedType, unexpectedValue, nil
	}
	if line == "Elevation underway" {
		secondLine, sndErr := conn.getServerResponse()
		if sndErr != nil {
			return Nil, nil, err
		}
		splitLine := strings.Split(secondLine, ":")
		if len(splitLine) != 2 || splitLine[0] != "Current level" {
			return Nil, nil, fmt.Errorf("Invalid level\n")
		}
		levelStr := strings.TrimSpace(splitLine[1])
		level, sndErr := strconv.Atoi(levelStr)
		if sndErr != nil {
			return Nil, nil, fmt.Errorf("Invalid level\n")
		}
		return Elevation, level, nil
	}
	return Nil, nil, fmt.Errorf("Invalid line\n")
}

// isResponseTypeValid checks if the type of the obtained response is expected for the last command sent
func (conn ServerConn) isResponseTypeValid(msgType MessageType) bool {
	for _, validType := range validResponsesTypes[conn.LastCommandType] {
		if validType == msgType {
			return true
		}
	}
	return false
}
