package ai

import (
	"log"
	"zappy_ai/network"
)

var levelUpRequirements = map[int]int{1: 6, 2: 5, 3: 5, 4: 4, 5: 4, 6: 4, 7: 4}

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
	if game.isAloneOnCurrentTile() {
		notAloneOnTileCounter = 0
	} else {
		notAloneOnTileCounter += 1
	}
}

func (game *Game) isAloneOnCurrentTile() bool {
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updatePriorityQueueAfterCollection()
	tile := game.View[0]
	playerCount := 0
	for _, resource := range tile {
		if resource == Player {
			playerCount++
		}
	}
	return playerCount == 1
}

func (game *Game) emergencyNotAloneOnTile() {
	newDir := network.PlayerDirection(game.Coordinates.CoordsFromOrigin[0]+
		game.Coordinates.CoordsFromOrigin[1]) + game.Coordinates.Direction
	newDir = newDir % 4
	dest := game.Coordinates.CoordsFromOrigin
	switch newDir {
	case Up:
		dest = RelativeCoordinates{dest[0], dest[1] + 1}
	case Down:
		dest = RelativeCoordinates{dest[0], dest[1] - 1}
	case Left:
		dest = RelativeCoordinates{dest[0] - 1, dest[1]}
	case Right:
		dest = RelativeCoordinates{dest[0] + 1, dest[1]}
	default:
	}
	game.moveToTile(dest)
	game.Socket.SendCommand(network.LookAround, network.EmptyBody) // Ask server for a view map
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.updatePrioritiesFromViewMap() // Update the priorities using the viewmap
	game.collectCurrentTileResources()
	if game.isAloneOnCurrentTile() {
		notAloneOnTileCounter = 0
	} else {
		notAloneOnTileCounter += 1
	}
}

// The game MainLoop
func (game *Game) MainLoop() {
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updatePrioritiesFromViewMap()
	game.collectCurrentTileResources()
	var path = Path{path: nil}
	log.Println("Is frequency command available :", FrequencyCommandAvailable)
	levelUpInitialRequirement := 6
	for getFoodPriority(&game.FoodManager.FoodPriority) > 0 {
		if FrequencyCommandAvailable { // Unnecessary if frequency command is unavailable
			game.Socket.SendCommand(network.GetInventory, network.EmptyBody)
			_ = game.awaitResponseToCommand()
		}
		game.updateFrequency()
		log.Println("Player current position", game.Coordinates.CoordsFromOrigin, "direction", game.Coordinates.Direction)
		if notAloneOnTileCounter > 6 && getFoodPriority(&game.FoodManager.FoodPriority) >= 8 {
			log.Println("Warning ! Low food level while on the same tile as a player for", notAloneOnTileCounter, "moves")
			game.emergencyNotAloneOnTile()
			path.path = nil
		} else if getFoodPriority(&game.FoodManager.FoodPriority) < levelUpInitialRequirement &&
			game.isLevelUpLeechAvailable() && game.Level < 8 { // Leeching is always easier, so it's more important
			log.Println("Started leeching")
			leechIdx := game.getLevelUpLeechIndex()
			if leechIdx == -1 {
				log.Println("Failed to get leech index")
				continue
			}
			leechMsg := game.MessageManager.messageStatusList[leechIdx]
			if leechMsg.direction == 0 { // Is on the correct tile?
				game.levelUpLeechLoop(leechMsg.uuid)
				levelUpInitialRequirement = levelUpRequirements[game.Level]
			} else {
				path.path = nil
				game.followMessageDirection(leechMsg.direction)
			}
			// Start leveling up as host
		} else if getFoodPriority(&game.FoodManager.FoodPriority) < levelUpInitialRequirement &&
			game.isLevelUpHostAvailable() && game.Level < 8 && game.isAloneOnCurrentTile() {
			log.Println("Started level up host")
			game.levelUpHostLoop()
			levelUpInitialRequirement = levelUpRequirements[game.Level]
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
	log.Println("Died of starvation, exiting...")
}
