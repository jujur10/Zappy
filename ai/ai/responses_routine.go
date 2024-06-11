package ai

import (
	"log"
	"time"
	"zappy_ai/network"
)

func switchMessageTypes(msgType network.MessageType, message any, game *Game) {
	switch msgType {
	case network.Death:
	case network.Nil:
	case network.Broadcast:
		game.InterpretPlayerMessage(message.(network.BroadcastData))
	case network.Int:
		if game.Socket.LastCommandType == network.GetUnusedSlots {
			game.SlotsLeft = message.(int)
		}
	case network.Direction:
		game.Coordinates.Direction = message.(network.PlayerDirection)
	case network.Frequency:
		game.TimeStep = time.Second / time.Duration(message.(int))
		game.FoodManager.TimeStepChannel <- game.TimeStep
	case network.Boolean:
		if message.(bool) == false {
			if game.Socket.LastCommandType == network.LevelUp {
				log.Println("Level up failed: conditions not met?")
				// TODO handle failed level up
				break
			}
			log.Println("Error on previous (invalid?) command: Server returned 'ko'")
		}
	case network.Inventory:
		inv, invErr := CreateInventory(message.(map[string]int))
		if invErr != nil {
			log.Println("Error: invalid inventory", invErr)
			break
		}
		game.Inventory = inv
		game.FoodManager.FoodChannel <- -game.Inventory[Food]
	case network.View:
		view, viewErr := CreateViewMap(message.([][]string))
		if viewErr != nil {
			log.Println("Error: invalid view", viewErr)
		}
		game.View = view
	case network.Elevation:
		level := message.(int)
		if level == 0 {
			break
			// TODO note level up process start
		}
	// TODO handle level up success
	default:
		log.Println("Error: unknown msgType", msgType)
	}
}

func serverResponseRoutine(statusChannel chan int, game *Game) {
	for {
		select {
		case _, ok := <-statusChannel:
			if !ok {
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
		switchMessageTypes(msgType, message, game)
	}
}
