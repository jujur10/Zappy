package ai

import (
	"log"
	"time"
	"zappy_ai/network"
)

func switchResponseTypes(msgType network.MessageType, message any, game *Game, feedbackChannel chan<- bool) {
	switch msgType {
	case network.Death:
	case network.Nil:
	case network.Broadcast:
		game.InterpretPlayerMessage(message.(network.BroadcastData))
	case network.Int:
		feedbackChannel <- true
		if game.Socket.LastCommandType == network.GetUnusedSlots {
			game.SlotsLeft = message.(int)
		}
	case network.Direction:
		feedbackChannel <- true
		game.Coordinates.Direction = message.(network.PlayerDirection)
	case network.Frequency:
		feedbackChannel <- true
		game.TimeStep = time.Second / time.Duration(message.(int))
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
		feedbackChannel <- true
		inv, invErr := CreateInventory(message.(map[string]int))
		if invErr != nil {
			log.Println("Error: invalid inventory", invErr)
			break
		}
		game.Inventory = inv
		game.FoodManager.FoodChannel <- -game.Inventory[Food]
	case network.View:
		feedbackChannel <- true
		view, viewErr := CreateViewMap(message.([][]string))
		if viewErr != nil {
			log.Println("Error: invalid view", viewErr)
		}
		game.View = view
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

func serverResponseRoutine(feedbackChannel chan bool, game *Game) {
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

func awaitResponseToCommand(feedbackChannel <-chan bool) bool {
	select {
	case value, ok := <-feedbackChannel:
		if ok {
			return value
		}
	}
	return false
}
