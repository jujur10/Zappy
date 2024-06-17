package ai

import (
	"log"
	"zappy_ai/network"
)

// The defaultAction of the player
func (game Game) defaultAction() {
	game.movePlayerForward()
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updatePrioritiesFromViewMap()
}

// The game MainLoop
func (game Game) MainLoop() {
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updatePrioritiesFromViewMap()
	var path = Path{path: nil}
	for game.FoodManager.FoodPriority > 0 && game.Level < 8 {
		if game.isLevelUpLeechAvailable() { // Leeching is always easier, so it's more important
			leechIdx := game.getLevelUpLeechIndex()
			if leechIdx == -1 {
				log.Println("Failed to get leech index")
				continue
			}
			leechMsg := game.MessageManager.messageStatusList[leechIdx]
			if leechMsg.direction == 0 { // Is on the correct tile?
				game.levelUpLeechLoop()
			}
			game.followMessageDirection(leechMsg.direction)
		} else if game.areLevelUpConditionsMet() { // Start leveling up as host
			game.levelUpHostLoop()
		} else if len(game.Movement.TilesQueue) != 0 && path.path == nil { // If there is no path configured
			newPath, err := game.computePath()
			if err != nil {
				log.Println(err)
				game.defaultAction()
				continue
			}
			path = game.followPath(newPath)
		} else if path.path != nil { // If there is an already existing path
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
			game.defaultAction()
		}
	}
	if game.Level == 8 { // Status messages on exit
		log.Println("Reached maximum level, exiting...")
	} else {
		log.Println("Died of starvation, exiting...")
	}
}
