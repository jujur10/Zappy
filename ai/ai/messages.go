package ai

import (
	"errors"
	"fmt"
	"strconv"
	"strings"
	"zappy_ai/network"
)

type broadcastType int

const (
	missingPlayers broadcastType = iota
	cancelLvlUp
	announcePresence
	startLvlUp
	lvlUpComplete
	lvlUpFailed
)

type broadcastMessageContent struct {
	msgType        broadcastType
	targetLevel    int
	missingPlayers int
	uuid           string
}

func levelUpReadyMissingPlayers(game Game, targetLevel, playersMissing int) {
	formatStr := fmt.Sprintf(" Ready %d, missing %d\n", targetLevel, playersMissing)
	game.Socket.SendCommand(network.BroadcastText, game.UUID+formatStr)
}

func cancelLevelUp(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Cancel %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.UUID+formatStr)
}

func announcePresenceLevelUp(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Join %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.UUID+formatStr)
}

func startLevelUp(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Starting %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.UUID+formatStr)
}

func levelUpComplete(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Reached %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.UUID+formatStr)
}

func levelUpFailed(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Failed %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.UUID+formatStr)
}

func interpretMessage(message string) (broadcastMessageContent, error) {
	uuid, message, found := strings.Cut(message, " ")
	if !found {
		return broadcastMessageContent{}, errors.New("invalid message format")
	}
	if strings.HasPrefix(message, "Ready ") {
		message = strings.TrimPrefix(message, "Ready ")
		messageParts := strings.Split(message, ",")
		if len(messageParts) != 2 || !strings.HasPrefix(messageParts[1], " missing ") {
			return broadcastMessageContent{}, fmt.Errorf("invalid message format: %s", message)
		}
		messageParts[1] = strings.TrimPrefix(messageParts[1], " missing ")
		level, err := strconv.Atoi(messageParts[0])
		if err != nil {
			return broadcastMessageContent{}, err
		}
		missing, err := strconv.Atoi(messageParts[1])
		if err != nil {
			return broadcastMessageContent{}, err
		}
		return broadcastMessageContent{msgType: missingPlayers, targetLevel: level, missingPlayers: missing, uuid: uuid}, nil
	}

	if strings.HasPrefix(message, "Cancel ") {
		message = strings.TrimPrefix(message, "Cancel ")
		level, err := strconv.Atoi(message)
		if err != nil {
			return broadcastMessageContent{}, err
		}
		return broadcastMessageContent{msgType: cancelLvlUp, targetLevel: level, uuid: uuid}, nil
	}

	if strings.HasPrefix(message, "Join ") {
		message = strings.TrimPrefix(message, "Join ")
		level, err := strconv.Atoi(message)
		if err != nil {
			return broadcastMessageContent{}, err
		}
		return broadcastMessageContent{msgType: announcePresence, targetLevel: level, uuid: uuid}, nil
	}

	if strings.HasPrefix(message, "Starting ") {
		message = strings.TrimPrefix(message, "Starting ")
		level, err := strconv.Atoi(message)
		if err != nil {
			return broadcastMessageContent{}, err
		}
		return broadcastMessageContent{msgType: startLvlUp, targetLevel: level, uuid: uuid}, nil
	}

	if strings.HasPrefix(message, "Reached ") {
		message = strings.TrimPrefix(message, "Reached ")
		level, err := strconv.Atoi(message)
		if err != nil {
			return broadcastMessageContent{}, err
		}
		return broadcastMessageContent{msgType: lvlUpComplete, targetLevel: level, uuid: uuid}, nil
	}

	if strings.HasPrefix(message, "Failed ") {
		message = strings.TrimPrefix(message, "Failed ")
		level, err := strconv.Atoi(message)
		if err != nil {
			return broadcastMessageContent{}, err
		}
		return broadcastMessageContent{msgType: lvlUpFailed, targetLevel: level, uuid: uuid}, nil
	}

	return broadcastMessageContent{}, fmt.Errorf("invalid message: %s", message)
}
