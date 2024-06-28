package ai

import (
	"fmt"
	"io"
	"log"
	"math"
	"time"
	"zappy_ai/network"
)

var FrequencyCommandAvailable = true

func checkChannelOpen[T any](channel chan T) bool {
	select {
	case _, ok := <-channel:
		return ok
	default:
	}
	return true
}

// switchResponseTypes runs the defined action for the given response type and updates the feedback channel
func switchResponseTypes(msgType network.MessageType, message any, game *Game, feedbackChannel chan<- bool) {
	switch msgType {
	case network.Death:
		log.Fatal("Died of starvation (received from server), exiting...")
	case network.Nil:
	case network.Broadcast:
		game.InterpretPlayerMessage(message.(network.BroadcastData))
	case network.Int:
		if game.Socket.LastCommandType == network.GetUnusedSlots {
			game.SlotsLeft = message.(int)
		}
		feedbackChannel <- true
	case network.Direction:
		game.Coordinates.Direction = message.(network.PlayerDirection)
		feedbackChannel <- true
	case network.Frequency:
		game.TimeStep = time.Second / time.Duration(message.(int))
		feedbackChannel <- true
		game.FoodManager.TimeStepChannel <- game.TimeStep
	case network.Boolean:
		feedbackChannel <- message.(bool)
		if message.(bool) == false {
			if game.Socket.LastCommandType == network.LevelUp {
				log.Println("Level up failed: conditions not met?")
			}
			log.Println("Error on previous (invalid?) command: Server returned 'ko'")
		}
	case network.Inventory:
		inv, invErr := createInventory(message.(map[string]int))
		if invErr != nil {
			log.Println("Error: invalid inventory", invErr)
			feedbackChannel <- false
			break
		}
		game.Inventory = inv
		feedbackChannel <- true
		game.FoodManager.InputFoodChannel <- -game.Inventory[Food]
	case network.View:
		view, viewErr := CreateViewMap(message.([][]string))
		if viewErr != nil {
			log.Println("Error: invalid view", viewErr)
		}
		game.View = view
		feedbackChannel <- viewErr == nil
	case network.Elevation:
		if game.Socket.IsResponseTypeValid(msgType) {
			feedbackChannel <- true
		}
		level := message.(int)
		if level == 0 {
			log.Println("Starting level up to level", game.Level+1)
			break
		}
		game.Level = level
		log.Println("Successfully leveled up to level", game.Level)
	default:
		log.Println("Error: unknown msgType", msgType)
	}
}

// serverResponseRoutine is the goroutine that manages the server's responses
func serverResponseRoutine(feedbackChannel chan bool, game *Game) {
	log.Println("Starting server response routine")
	for {
		msgType, message, err := game.Socket.GetAndParseResponse()
		if err != nil {
			log.Println("Error getting response", err)
			if err == io.EOF {
				log.Fatal("Got EOF when reading from socket")
			}
		}
		log.Println("Got response from server of type", network.MessageTypesToString[msgType], "containing", message)
		if game.Socket.IsResponseTypeValid(msgType) == false && msgType != network.Elevation {
			log.Println("Invalid response type", msgType, "previous command was", game.Socket.LastCommandType)
			continue
		}
		if !checkChannelOpen(feedbackChannel) {
			log.Fatalln("Server response handling : channel closed, exiting..")
			return
		}
		switchResponseTypes(msgType, message, game, feedbackChannel)
	}
}

// updateFrequency by requesting it from the server
func (game *Game) updateFrequency() {
	if FrequencyCommandAvailable {
		game.Socket.SendCommand(network.GetFrequency, network.EmptyBody)
		select {
		case _ = <-game.Socket.ResponseFeedbackChannel:
		}
	} else {
		timingSum := time.Duration(0)
		for range 5 {
			timerStart := time.Now()
			game.Socket.SendCommand(network.GetInventory, network.EmptyBody)
			select {
			case _ = <-game.Socket.ResponseFeedbackChannel:
			}
			timingSum += time.Since(timerStart)
		}
		timeStepFloat := float64(time.Second.Microseconds()) / float64(timingSum.Microseconds()/5)
		game.TimeStep = time.Second / time.Duration(int(math.Round(timeStepFloat)))
		fmt.Println("New approximated timestep:", game.TimeStep, "; frequency:", int(math.Round(timeStepFloat)))
	}
}

// awaitResponseToCommand is used to wait until the server returns a response to the command
func (game *Game) awaitResponseToCommand() bool {
	responseValue := false
	select {
	case value, ok := <-game.Socket.ResponseFeedbackChannel:
		if ok {
			responseValue = value
		}
	}

	return responseValue
}
