package ai

import (
	"log"
	"zappy_ai/network"
)

func (game *Game) collectCurrentTileResources() {
	currentTilePtr := GetPriorityQueueItem(&game.Movement.TilesQueue, game.Coordinates.CoordsFromOrigin)
	if currentTilePtr != nil {
		game.collectTileResources(currentTilePtr)
		RemoveFromPriorityQueue(&game.Movement.TilesQueue, currentTilePtr.value)
		game.updatePriorityQueueAfterCollection()
	}
}

// The defaultAction of the player
func (game *Game) defaultAction() {
	game.movePlayerForward()
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.updatePrioritiesFromViewMap()
	game.collectCurrentTileResources()
}

// The game MainLoop
func (game *Game) MainLoop() {
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updatePrioritiesFromViewMap()
	game.collectCurrentTileResources()
	var path = Path{path: nil}
	log.Println("Is frequency command available :", FrequencyCommandAvailable)
	for getFoodPriority(&game.FoodManager.FoodPriority) > 0 && game.Level < 8 {
		if FrequencyCommandAvailable { // Unnecessary if frequency command is unavailable
			game.Socket.SendCommand(network.GetInventory, network.EmptyBody)
			_ = game.awaitResponseToCommand()
		}
		game.updateFrequency()
		log.Println("Player current position", game.Coordinates.CoordsFromOrigin, "direction", game.Coordinates.Direction)
		// Leeching is always easier, so it's more important
		if game.FoodManager.FoodPriority < 7 && game.isLevelUpLeechAvailable() {
			log.Println("Started leeching")
			leechIdx := game.getLevelUpLeechIndex()
			if leechIdx == -1 {
				log.Println("Failed to get leech index")
				continue
			}
			leechMsg := game.MessageManager.messageStatusList[leechIdx]
			if leechMsg.direction == 0 { // Is on the correct tile?
				game.levelUpLeechLoop(leechMsg.uuid)
			} else {
				path.path = nil
				game.followMessageDirection(leechMsg.direction)
			}
		} else if game.isLevelUpHostAvailable() { // Start leveling up as host
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
			if len(path.path) == 0 && path.destination.value == game.Coordinates.CoordsFromOrigin {
				log.Println("Destroyed path with destination", path.destination)
				path.path = nil
			}
		} else if path.path != nil { // If there is an already existing path
			log.Println("Following existing path")
			if len(game.Movement.TilesQueue) > 0 {
				destinationPrio := max(0, path.destination.originalPriority-
					ManhattanDistance(game.Coordinates.CoordsFromOrigin, path.destination.value,
						game.Coordinates.WorldSize))
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
		log.Println("Died of starvation, exiting...")
	}
}
