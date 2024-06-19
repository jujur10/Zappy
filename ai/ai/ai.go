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
	game.updateFrequency()
	game.updatePrioritiesFromViewMap()
	var path = Path{path: nil}
	for game.FoodManager.FoodPriority > 0 && game.Level < 8 {
		game.Socket.SendCommand(network.GetInventory, network.EmptyBody) // Fetch the inventory to update food level
		_ = game.awaitResponseToCommand()

		game.updateFrequency()
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
			log.Println("Creating new path")
			newPath, err := game.computePath()
			if err != nil {
				log.Println(err)
				game.defaultAction()
				continue
			}
			path = game.followPath(newPath)
		} else if path.path != nil { // If there is an already existing path
			log.Println("Following existing path")
			destinationPrio := max(0, path.destination.originalPriority-
				ManhattanDistance(game.Coordinates.CoordsFromOrigin, path.destination.value))
			pqHead := *game.Movement.TilesQueue[0]
			path.destination.priority = destinationPrio
			if pqHead.originalPriority > path.destination.originalPriority && pqHead.priority > destinationPrio {
				newPath, err := game.computePath()
				if err != nil {
					log.Println(err)
				} else {
					path = newPath
				}
				path = game.followPath(path)
				if len(path.path) == 0 && path.destination.value == game.Coordinates.CoordsFromOrigin {
					path.path = nil
				}
			}
		} else { // If no other option is available, just move forward
			log.Println("Default action")
			game.defaultAction()
		}
	}
	if game.Level == 8 { // Status messages on exit
		log.Println("Reached maximum level, exiting...")
	} else {
		log.Println("Food prio", game.FoodManager.FoodPriority)
		log.Println("Died of starvation, exiting...")
	}
}
