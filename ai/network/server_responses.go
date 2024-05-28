package network

import (
	"fmt"
	"strconv"
	"strings"
)

const (
	Boolean MessageType = iota
	Inventory
	View
	Int
	Elevation
	Death
	Nil
)

var inventoryIndexes = []string{"food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"}
var validObjects = []string{"food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame", "player"}

func (conn ServerConn) getServerResponse() (string, error) {
	line, err := conn.Reader.ReadLine()
	if err != nil {
		return "", err
	}
	return line, nil
}

// T est une "template" contrainte par le keyword 'comparable'.
func IsInArray[T comparable](value T, array []T) bool {
	for _, v := range array {
		if v == value {
			return true
		}
	}
	return false
}

func checkValues(values [][]string) bool {
	for _, tile := range values {
		for _, val := range tile {
			if !IsInArray(val, validObjects) {
				return false
			}
		}
	}
	return true
}

func ParseArray(line string) (MessageType, any, error) {
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
		inventory := make(map[string]int)
		valid := true

		for _, pair := range individualValues {
			val, err := strconv.Atoi(pair[1])
			if !IsInArray(pair[0], inventoryIndexes) || err != nil {
				valid = false
				break
			}
			inventory[pair[0]] = val
		}
		if valid {
			return Inventory, inventory, nil
		}
	}

	if checkValues(individualValues) == false {
		return Nil, nil, fmt.Errorf("Invalid values\n")
	}

	return View, values, nil
}

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
	if line == "dead" {
		return Death, nil, nil
	}
	if line[0] == '[' && line[len(line)-1] == ']' {
		return ParseArray(line)
	}
	val, err := strconv.Atoi(line)
	if err == nil {
		return Int, val, nil
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
