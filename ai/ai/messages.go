package ai

import (
	"errors"
	"fmt"
	"log"
	"strconv"
	"strings"
	"zappy_ai/network"
)

type broadcastType int

const (
	missingPlayers broadcastType = iota
	cancelLvlUp
	announcePresence
	announceDeparture
	startLvlUp
	lvlUpComplete
	lvlUpFailed
)

type broadcastMessageContent struct {
	// msgType is the type of the broadcast
	msgType broadcastType
	// targetLevel is the level to reach
	targetLevel int
	// missingPlayers is the number of players missing to start the level up, if applicable
	missingPlayers int
	// the uuid of the message sender
	uuid string
	// the direction the message is coming from
	direction network.EventDirection
}

// levelUpReadyMissingPlayers sends a formatted missingPlayers message
func levelUpReadyMissingPlayers(game *Game, targetLevel, playersMissing int) {
	formatStr := fmt.Sprintf(" Ready %d, missing %d\n", targetLevel, playersMissing)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = true
}

// cancelLevelUp sends a formatted cancelLvlUp message
func cancelLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Cancel %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

// announcePresenceLevelUp sends a formatted announcePresence message
func announcePresenceLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Join %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUpLeech = true
	game.MessageManager.waitingForLevelUp = false
}

// announceDepartureLevelUp sends a formatted announceDeparture message
func announceDepartureLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Leave %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUpLeech = true
}

// startLevelUp sends a formatted startLvlUp message
func startLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Starting %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

// levelUpComplete sends a formatted lvlUpComplete message
func levelUpComplete(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Reached %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

// levelUpFailed sends a formatted lvlUpFailed message
func levelUpFailed(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Failed %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

// parseMessageLevelAndReturn parses the target level of the messages and returns a broadcastMessageContent struct
func parseMessageLevelAndReturn(levelStr string, uuid string, msgType broadcastType) (broadcastMessageContent, error) {
	level, err := strconv.Atoi(levelStr)
	if err != nil || level < 2 || level > 8 {
		if level < 2 || level > 8 {
			err = fmt.Errorf("invalid level %d", level)
		}
		return broadcastMessageContent{}, err
	}
	return broadcastMessageContent{msgType: msgType, targetLevel: level, uuid: uuid}, nil
}

// parsePlayerMessage parses a message coming from another player.
// It's not that complicated, it's just a lot of duplicated ifs
func parsePlayerMessage(message string) (broadcastMessageContent, error) {
	uuid, message, found := strings.Cut(message, " ")
	if !found {
		return broadcastMessageContent{}, errors.New("invalid message format")
	}
	if strings.HasPrefix(message, "Ready ") { // Check if it's a missingPlayers message
		message = strings.TrimPrefix(message, "Ready ")
		messageParts := strings.Split(message, ",")
		if len(messageParts) != 2 || !strings.HasPrefix(messageParts[1], " missing ") {
			return broadcastMessageContent{}, fmt.Errorf("invalid message format: %s", message)
		}
		messageParts[1] = strings.TrimPrefix(messageParts[1], " missing ")
		broadcast, err := parseMessageLevelAndReturn(messageParts[0], uuid, missingPlayers)
		missing, err := strconv.Atoi(messageParts[1])
		if missing < 1 || missing > levelUpResources[broadcast.targetLevel-1][Player] {
			err = fmt.Errorf("invalid missing value %d", missing)
		}
		if err != nil {
			return broadcastMessageContent{}, err
		}
		broadcast.missingPlayers = missing
		return broadcast, nil
	}

	if strings.HasPrefix(message, "Cancel ") {
		message = strings.TrimPrefix(message, "Cancel ")
		return parseMessageLevelAndReturn(message, uuid, cancelLvlUp)
	}

	if strings.HasPrefix(message, "Join ") {
		message = strings.TrimPrefix(message, "Join ")
		return parseMessageLevelAndReturn(message, uuid, announcePresence)
	}

	if strings.HasPrefix(message, "Leave ") {
		message = strings.TrimPrefix(message, "Leave ")
		return parseMessageLevelAndReturn(message, uuid, announceDeparture)
	}

	if strings.HasPrefix(message, "Starting ") {
		message = strings.TrimPrefix(message, "Starting ")
		return parseMessageLevelAndReturn(message, uuid, startLvlUp)
	}

	if strings.HasPrefix(message, "Reached ") {
		message = strings.TrimPrefix(message, "Reached ")
		return parseMessageLevelAndReturn(message, uuid, lvlUpComplete)
	}

	if strings.HasPrefix(message, "Failed ") {
		message = strings.TrimPrefix(message, "Failed ")
		return parseMessageLevelAndReturn(message, uuid, lvlUpFailed)
	}

	return broadcastMessageContent{}, fmt.Errorf("invalid message: %s", message)
}

// getMessageIndex using the message sender's UUID as a reference
func getMessageIndex(content broadcastMessageContent, messageList []broadcastMessageContent) int {
	for i, message := range messageList {
		if content.uuid == message.uuid {
			return i
		}
	}
	return -1
}

// InterpretPlayerMessage parses the broadcast message and handles its content as necessary
func (game *Game) InterpretPlayerMessage(message network.BroadcastData) {
	messageContent, err := parsePlayerMessage(message.Text)
	if err != nil {
		log.Println("Error parsing player message:", err)
	}

	messageContent.direction = message.Direction
	messageIndex := getMessageIndex(messageContent, game.MessageManager.messageStatusList)
	if messageIndex == -1 {
		if messageContent.msgType == missingPlayers { // New level up lobby
			game.MessageManager.messageStatusList = append(game.MessageManager.messageStatusList, messageContent)
			return
		}
		log.Println("Invalid message, message not in array: ", message.Text)
		return
	}
	if messageContent.msgType == missingPlayers { // Update to a level up lobby
		game.MessageManager.messageStatusList[messageIndex] = messageContent
	} else if messageContent.msgType == cancelLvlUp || // Closed level up lobby
		messageContent.msgType == lvlUpComplete ||
		messageContent.msgType == lvlUpFailed {
		if game.MessageManager.waitingForLevelUpLeech {
			game.MessageManager.levelUpMessageChannel <- messageContent
		}
		game.MessageManager.messageStatusList = append(game.MessageManager.messageStatusList[:messageIndex],
			game.MessageManager.messageStatusList[messageIndex+1:]...)
	}
	if game.MessageManager.waitingForLevelUp && // Player joined / left the lobby
		messageContent.targetLevel == game.Level+1 &&
		(messageContent.msgType == announcePresence || messageContent.msgType == announceDeparture) {
		game.MessageManager.levelUpMessageChannel <- messageContent
	}
	if game.MessageManager.waitingForLevelUpLeech && // Host's lobby update
		messageContent.targetLevel == game.Level+1 &&
		(messageContent.msgType == startLvlUp || messageContent.msgType == cancelLvlUp) {
		game.MessageManager.levelUpMessageChannel <- messageContent
	}
}
