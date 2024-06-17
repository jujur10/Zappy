package ai

import (
	"log"
	"time"
	"zappy_ai/network"
)

// switchResponseTypes runs the defined action for the given response type and updates the feedback channel
func switchResponseTypes(msgType network.MessageType, message any, game *Game, feedbackChannel chan<- bool) {
	switch msgType {
	case network.Death:
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
		game.FoodManager.FoodChannel <- -game.Inventory[Food]
	case network.View:
		view, viewErr := CreateViewMap(message.([][]string))
		if viewErr != nil {
			log.Println("Error: invalid view", viewErr)
		}
		game.View = view
		feedbackChannel <- viewErr == nil
	case network.Elevation:
		feedbackChannel <- true
		level := message.(int)
		if level == 0 {
			log.Println("Starting level up to level ", game.Level+1)
			break
		}
		log.Println("Successfully leveled up to level ", game.Level+1)
	default:
		log.Println("Error: unknown msgType", msgType)
	}
}

// serverResponseRoutine is the goroutine that manages the server's responses
func serverResponseRoutine(feedbackChannel chan bool, game *Game) {
	log.Println("Starting server response routine")
	for {
		select {
		case _, ok := <-feedbackChannel:
			if !ok {
				log.Println("Server response handling : channel closed, exiting..")
				return
			}
		default:
		}
		msgType, message, err := game.Socket.GetAndParseResponse()
		if err != nil {
			log.Println("Error getting response", err)
		}
		if game.Socket.IsResponseTypeValid(msgType) == false {
			log.Println("Invalid response type", msgType)
		}
		switchResponseTypes(msgType, message, game, feedbackChannel)
	}
}

// awaitResponseToCommand is used to wait until the server returns a response to the command
func (game Game) awaitResponseToCommand() bool {
	responseValue := false
	select {
	case value, ok := <-game.Socket.ResponseFeedbackChannel:
		if ok {
			responseValue = value
		}
	}
	select {
	case priority, ok := <-game.FoodManager.FoodChannel: // Check if there is an update of the food priority
		if ok {
			game.FoodManager.FoodPriority = priority
			game.updatePrioritiesFromViewMap() // Recompute priorities
		}
	default:
	}
	game.Socket.SendCommand(network.GetFrequency, network.EmptyBody)
	select {
	case _ = <-game.Socket.ResponseFeedbackChannel:
	}
	return responseValue
}
