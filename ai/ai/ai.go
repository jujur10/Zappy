package ai

import (
	"log"
	"zappy_ai/network"
)

// The defaultAction of the player
func (game *Game) defaultAction() {
	game.movePlayerForward()
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.updatePrioritiesFromViewMap()
}

// The game MainLoop
func (game *Game) MainLoop() {
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updatePrioritiesFromViewMap()
	for _, item := range game.Movement.TilesQueue {
		log.Print("PQueue element coords ", item.value, " original priority ", item.originalPriority, " index ", item.index)
		for _, usefulItem := range item.usefulObjects {
			log.Print(" ", itemToString[usefulItem])
		}
	}
	pqTileItem := GetPriorityQueueItem(&game.Movement.TilesQueue, game.Coordinates.CoordsFromOrigin)
	if pqTileItem != nil { // Remove tile if it was in the queue
		game.collectTileResources(pqTileItem)
		RemoveFromPriorityQueue(&game.Movement.TilesQueue, pqTileItem.value)
		game.updatePriorityQueueAfterCollection()
	}
	var path = Path{path: nil}
	log.Println("Is frequency command available :", FrequencyCommandAvailable)
	for getFoodPriority(&game.FoodManager.FoodPriority) > 0 && game.Level < 8 {
		if FrequencyCommandAvailable { // Unnecessary if frequency command is unavailable
			game.Socket.SendCommand(network.GetInventory, network.EmptyBody)
			_ = game.awaitResponseToCommand()
		}
		game.updateFrequency()
		log.Println("Player current position", game.Coordinates.CoordsFromOrigin, "direction", game.Coordinates.Direction)
		for _, item := range game.Movement.TilesQueue {
			log.Print("PQueue element coords ", item.value, " original priority ", item.originalPriority, " index ", item.index)
			for _, usefulItem := range item.usefulObjects {
				log.Print(" ", itemToString[usefulItem])
			}
		}

		if game.isLevelUpLeechAvailable() { // Leeching is always easier, so it's more important
			log.Println("Started leeching")
			leechIdx := game.getLevelUpLeechIndex()
			if leechIdx == -1 {
				log.Println("Failed to get leech index")
				continue
			}
			leechMsg := game.MessageManager.messageStatusList[leechIdx]
			if leechMsg.direction == 0 { // Is on the correct tile?
				game.levelUpLeechLoop(leechMsg.uuid)
			}
			game.followMessageDirection(leechMsg.direction)
		} else if game.areLevelUpConditionsMet() { // Start leveling up as host
			log.Println("Started level up host")
			game.levelUpHostLoop()
		} else if len(game.Movement.TilesQueue) != 0 && path.path == nil { // If there is no path configured
			newPath, err := game.computePath()
			if err != nil {
				log.Println(err)
				game.defaultAction()
				continue
			}
			log.Println("Created new path; destination", newPath.destination, "path", newPath.path)
			path = game.followPath(newPath)
			log.Println("Moved on path; destination", newPath.destination, "path", path.path)
			if len(path.path) == 0 && path.destination.value == game.Coordinates.CoordsFromOrigin {
				log.Println("Destroyed path with destination", path.destination)
				path.path = nil
			}
		} else if path.path != nil { // If there is an already existing path
			log.Println("Following existing path")
			if len(game.Movement.TilesQueue) > 0 {
				destinationPrio := max(0, path.destination.originalPriority-
					ManhattanDistance(game.Coordinates.CoordsFromOrigin, path.destination.value))
				path.destination.priority = destinationPrio
				pqHead := *game.Movement.TilesQueue[0]
				if pqHead.originalPriority > path.destination.originalPriority && pqHead.priority > destinationPrio {
					newPath, err := game.computePath()
					log.Println("Found more important tile, created new path", newPath.destination, "path", newPath.path)
					if err != nil {
						log.Println(err)
					} else {
						path = newPath
					}
				}
			}
			log.Println("Following path; destination", path.destination, "path", path.path)
			path = game.followPath(path)
			log.Println("Moved on path; destination", path.destination, "path", path.path)
			if len(path.path) == 0 && path.destination.value == game.Coordinates.CoordsFromOrigin {
				log.Println("Destroyed path with destination", path.destination)
				path.path = nil
			}
		} else { // If no other option is available, just move forward
			log.Println("Default action")
			game.defaultAction()
		}
	}
	if game.Level == 8 { // Status messages on exit
		log.Println("Reached maximum level, exiting...")
	} else {
		log.Println("Food prio", getFoodPriority(&game.FoodManager.FoodPriority))
		log.Println("Died of starvation, exiting...")
	}
}
