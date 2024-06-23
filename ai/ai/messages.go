package ai

import (
	"crypto/cipher"
	"crypto/rand"
	"encoding/hex"
	"errors"
	"fmt"
	"io"
	"log"
	"strconv"
	"strings"
	"sync"
	"zappy_ai/network"
)

type broadcastType int

var CipherMessages = true

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

type messageReceptionQueue struct {
	queue []broadcastMessageContent
	lock  sync.Mutex
}

var messageQueue = messageReceptionQueue{queue: make([]broadcastMessageContent, 0)}

func addMessageToQueue(message broadcastMessageContent) {
	messageQueue.lock.Lock()
	defer messageQueue.lock.Unlock()
	messageQueue.queue = append(messageQueue.queue, message)
}

func popMessageFromQueue() (broadcastMessageContent, error) {
	messageQueue.lock.Lock()
	defer messageQueue.lock.Unlock()
	if len(messageQueue.queue) == 0 {
		return broadcastMessageContent{}, fmt.Errorf("queue is empty")
	}
	message := messageQueue.queue[0]
	messageQueue.queue = messageQueue.queue[1:]
	return message, nil
}

func cipherMessage(message string, gcm cipher.AEAD) string {
	nonce := make([]byte, gcm.NonceSize())
	if _, err := io.ReadFull(rand.Reader, nonce); err != nil {
		log.Fatal("Failed to generate nonce", err)
	}
	cipherText := gcm.Seal(nonce, nonce, []byte(message), nil)
	return hex.EncodeToString(cipherText)
}

func decipherMessage(message string, gcm cipher.AEAD) string {
	cipherText, err := hex.DecodeString(message)
	if err != nil {
		log.Fatal("Failed to decode hex message", err)
	}
	decipheredData, err := gcm.Open(nil, cipherText[:gcm.NonceSize()], cipherText[gcm.NonceSize():], nil)
	if err != nil {
		log.Fatal("Failed to decrypt message", err)
	}
	return string(decipheredData)
}

// levelUpReadyMissingPlayers sends a formatted missingPlayers message
func levelUpReadyMissingPlayers(game *Game, targetLevel, playersMissing int) {
	formatStr := fmt.Sprintf("_Ready_%d,_missing_%d", targetLevel, playersMissing)
	message := game.MessageManager.UUID + formatStr
	if CipherMessages {
		message = cipherMessage(message, game.AesCipher)
	}
	game.Socket.SendCommand(network.BroadcastText, message)
	game.MessageManager.waitingForLevelUp = true
}

// cancelLevelUp sends a formatted cancelLvlUp message
func cancelLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf("_Cancel_%d", targetLevel)
	message := game.MessageManager.UUID + formatStr
	if CipherMessages {
		message = cipherMessage(message, game.AesCipher)
	}
	game.Socket.SendCommand(network.BroadcastText, message)
	game.MessageManager.waitingForLevelUp = false
}

// announcePresenceLevelUp sends a formatted announcePresence message
func announcePresenceLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf("_Join_%d", targetLevel)
	message := game.MessageManager.UUID + formatStr
	if CipherMessages {
		message = cipherMessage(message, game.AesCipher)
	}
	game.Socket.SendCommand(network.BroadcastText, message)
	game.MessageManager.waitingForLevelUpLeech = true
	game.MessageManager.waitingForLevelUp = false
}

// announceDepartureLevelUp sends a formatted announceDeparture message
func announceDepartureLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf("_Leave_%d", targetLevel)
	message := game.MessageManager.UUID + formatStr
	if CipherMessages {
		message = cipherMessage(message, game.AesCipher)
	}
	game.Socket.SendCommand(network.BroadcastText, message)
	game.MessageManager.waitingForLevelUp = false
	game.MessageManager.waitingForLevelUpLeech = false
}

// startLevelUp sends a formatted startLvlUp message
func startLevelUp(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf("_Starting_%d", targetLevel)
	message := game.MessageManager.UUID + formatStr
	if CipherMessages {
		message = cipherMessage(message, game.AesCipher)
	}
	game.Socket.SendCommand(network.BroadcastText, message)
	game.MessageManager.waitingForLevelUp = false
}

// levelUpComplete sends a formatted lvlUpComplete message
func levelUpComplete(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf("_Reached_%d", targetLevel)
	message := game.MessageManager.UUID + formatStr
	if CipherMessages {
		message = cipherMessage(message, game.AesCipher)
	}
	game.Socket.SendCommand(network.BroadcastText, message)
	game.MessageManager.waitingForLevelUp = false
}

// levelUpFailed sends a formatted lvlUpFailed message
func levelUpFailed(game *Game, targetLevel int) {
	formatStr := fmt.Sprintf("_Failed_%d", targetLevel)
	message := game.MessageManager.UUID + formatStr
	if CipherMessages {
		message = cipherMessage(message, game.AesCipher)
	}
	game.Socket.SendCommand(network.BroadcastText, message)
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
func parsePlayerMessage(message string, gcm cipher.AEAD) (broadcastMessageContent, error) {
	if CipherMessages {
		message = decipherMessage(message, gcm)
	}
	uuid, message, found := strings.Cut(message, "_")
	if !found {
		return broadcastMessageContent{}, errors.New("invalid message format")
	}
	if strings.HasPrefix(message, "Ready_") { // Check if it's a missingPlayers message
		message = strings.TrimPrefix(message, "Ready_")
		messageParts := strings.Split(message, ",")
		if len(messageParts) != 2 || !strings.HasPrefix(messageParts[1], "_missing_") {
			return broadcastMessageContent{}, fmt.Errorf("invalid message format: %s", message)
		}
		messageParts[1] = strings.TrimPrefix(messageParts[1], "_missing_")
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

	if strings.HasPrefix(message, "Cancel_") {
		message = strings.TrimPrefix(message, "Cancel_")
		return parseMessageLevelAndReturn(message, uuid, cancelLvlUp)
	}

	if strings.HasPrefix(message, "Join_") {
		message = strings.TrimPrefix(message, "Join_")
		return parseMessageLevelAndReturn(message, uuid, announcePresence)
	}

	if strings.HasPrefix(message, "Leave_") {
		message = strings.TrimPrefix(message, "Leave_")
		return parseMessageLevelAndReturn(message, uuid, announceDeparture)
	}

	if strings.HasPrefix(message, "Starting_") {
		message = strings.TrimPrefix(message, "Starting_")
		return parseMessageLevelAndReturn(message, uuid, startLvlUp)
	}

	if strings.HasPrefix(message, "Reached_") {
		message = strings.TrimPrefix(message, "Reached_")
		return parseMessageLevelAndReturn(message, uuid, lvlUpComplete)
	}

	if strings.HasPrefix(message, "Failed_") {
		message = strings.TrimPrefix(message, "Failed_")
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
	messageContent, err := parsePlayerMessage(message.Text, game.AesCipher)
	if err != nil {
		log.Println("Error parsing player message:", err)
	}

	messageContent.direction = message.Direction
	messageIndex := getMessageIndex(messageContent, game.MessageManager.messageStatusList)
	if messageIndex == -1 && messageContent.msgType == missingPlayers { // New level up lobby
		game.MessageManager.messageStatusList = append(game.MessageManager.messageStatusList, messageContent)
		return
	}

	if game.MessageManager.waitingForLevelUp && // Player joined / left the lobby
		messageContent.targetLevel == game.Level+1 &&
		(messageContent.msgType == announcePresence || messageContent.msgType == announceDeparture) {
		addMessageToQueue(messageContent)
	}
	if game.MessageManager.waitingForLevelUpLeech && // Host's lobby update
		messageContent.targetLevel == game.Level+1 &&
		(messageContent.msgType == startLvlUp || messageContent.msgType == cancelLvlUp ||
			messageContent.msgType == lvlUpComplete || messageContent.msgType == lvlUpFailed) {
		addMessageToQueue(messageContent)
	}

	if messageContent.msgType == missingPlayers { // Update to a level up lobby
		game.MessageManager.messageStatusList[messageIndex] = messageContent
	} else if messageIndex != -1 && (messageContent.msgType == cancelLvlUp || // Closed level up lobby
		messageContent.msgType == lvlUpComplete || messageContent.msgType == lvlUpFailed) {
		game.MessageManager.messageStatusList = append(game.MessageManager.messageStatusList[:messageIndex],
			game.MessageManager.messageStatusList[messageIndex+1:]...)
	}
}
