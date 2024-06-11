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
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = true
}

func cancelLevelUp(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Cancel %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

func announcePresenceLevelUp(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Join %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
}

func startLevelUp(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Starting %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

func levelUpComplete(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Reached %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

func levelUpFailed(game Game, targetLevel int) {
	formatStr := fmt.Sprintf(" Failed %d\n", targetLevel)
	game.Socket.SendCommand(network.BroadcastText, game.MessageManager.UUID+formatStr)
	game.MessageManager.waitingForLevelUp = false
}

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
	if strings.HasPrefix(message, "Ready ") {
		message = strings.TrimPrefix(message, "Ready ")
		messageParts := strings.Split(message, ",")
		if len(messageParts) != 2 || !strings.HasPrefix(messageParts[1], " missing ") {
			return broadcastMessageContent{}, fmt.Errorf("invalid message format: %s", message)
		}
		messageParts[1] = strings.TrimPrefix(messageParts[1], " missing ")
		broadcast, err := parseMessageLevelAndReturn(messageParts[0], uuid, missingPlayers)
		missing, err := strconv.Atoi(messageParts[1])
		if missing < 1 || missing > levelUpResources[broadcast.targetLevel][Player] {
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

func getMessageIndex(content broadcastMessageContent, messageList []broadcastMessageContent) int {
	for i, message := range messageList {
		if content.uuid == message.uuid {
			return i
		}
	}
	return -1
}

func (game Game) interpretPlayerMessage(message network.BroadcastData) {
	messageContent, err := parsePlayerMessage(message.Text)
	if err != nil {
		log.Println("Error parsing player message:", err)
	}

	messageIndex := getMessageIndex(messageContent, game.MessageManager.messageStatusList)
	if messageIndex == -1 {
		if messageContent.msgType == missingPlayers {
			game.MessageManager.messageStatusList = append(game.MessageManager.messageStatusList, messageContent)
			return
		}
		log.Println("Invalid message, message not in array: ", message.Text)
		return
	}
	if messageContent.msgType == missingPlayers {
		game.MessageManager.messageStatusList[messageIndex] = messageContent
	} else if messageContent.msgType == cancelLvlUp ||
		messageContent.msgType == lvlUpComplete ||
		messageContent.msgType == lvlUpFailed {
		game.MessageManager.messageStatusList = append(game.MessageManager.messageStatusList[:messageIndex],
			game.MessageManager.messageStatusList[messageIndex+1:]...)
	}
	if game.MessageManager.waitingForLevelUp &&
		game.MessageManager.waitLevelUpMsg != nil &&
		messageContent.msgType == announcePresence &&
		messageContent.targetLevel == game.Level+1 {
		(*game.MessageManager.waitLevelUpMsg).missingPlayers -= 1
		if (*game.MessageManager.waitLevelUpMsg).missingPlayers == 0 {
			startLevelUp(game, game.Level+1)
			// Todo start level up
		} else {
			levelUpReadyMissingPlayers(game, game.Level+1, (*game.MessageManager.waitLevelUpMsg).missingPlayers)
		}
	}
	if messageContent.msgType == startLvlUp &&
		message.Direction == 0 &&
		messageContent.targetLevel == game.Level+1 {
		// Todo start level up
	}
}
