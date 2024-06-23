package ai

import (
	"log"
	"zappy_ai/network"
)

// resourcesToDrop is a constant reference table for looking up which resources to drop before leveling up
var resourcesToDrop = map[int]Inventory{
	1: {Linemate: 1, Deraumere: 0, Sibur: 0, Mendiane: 0, Phiras: 0, Thystame: 0},
	2: {Linemate: 1, Deraumere: 1, Sibur: 1, Mendiane: 0, Phiras: 0, Thystame: 0},
	3: {Linemate: 2, Deraumere: 0, Sibur: 1, Mendiane: 0, Phiras: 2, Thystame: 0},
	4: {Linemate: 1, Deraumere: 1, Sibur: 2, Mendiane: 0, Phiras: 1, Thystame: 0},
	5: {Linemate: 1, Deraumere: 2, Sibur: 1, Mendiane: 3, Phiras: 0, Thystame: 0},
	6: {Linemate: 1, Deraumere: 2, Sibur: 3, Mendiane: 0, Phiras: 1, Thystame: 0},
	7: {Linemate: 2, Deraumere: 2, Sibur: 2, Mendiane: 2, Phiras: 2, Thystame: 1},
}

// itemToString converts TileItems to strings
var itemToString = map[TileItem]string{
	Player:    "player",
	Food:      "food",
	Linemate:  "linemate",
	Deraumere: "deraumere",
	Sibur:     "sibur",
	Mendiane:  "mendiane",
	Phiras:    "phiras",
	Thystame:  "thystame",
}

// getLevelUpLeechIndex returns the index of the message containing the possible level leech
func (game *Game) getLevelUpLeechIndex() int {
	if game.MessageManager.waitingForLevelUp {
		return -1
	}
	for idx, message := range game.MessageManager.messageStatusList {
		if message.msgType == missingPlayers &&
			message.uuid != game.MessageManager.UUID &&
			message.targetLevel == game.Level+1 {
			return idx
		}
	}
	return -1
}

// isLevelUpLeechAvailable checks if a hosted level up is available to leech from
func (game *Game) isLevelUpLeechAvailable() bool {
	if game.MessageManager.waitingForLevelUp {
		return false
	}
	for _, message := range game.MessageManager.messageStatusList {
		if message.msgType == missingPlayers &&
			message.uuid != game.MessageManager.UUID &&
			message.targetLevel == game.Level+1 {
			return true
		}
	}
	return false
}

// isLevelUpHostAvailable checks if it is possible to host a level up
func (game *Game) isLevelUpHostAvailable() bool {
	if game.MessageManager.waitingForLevelUpLeech {
		return false
	}
	if getFoodPriority(&game.FoodManager.FoodPriority) >= 7 {
		return false
	}
	levelUpMap := game.LevelUpResources[game.Level]
	for key, value := range levelUpMap {
		if key == Player || key == Food {
			continue
		}
		if value != 0 {
			return false
		}
	}
	return true
}

// dropResources drops the resources needed for a level up to the ground
func dropResources(game *Game) {
	levelResources := resourcesToDrop[game.Level]
	for resource, amount := range levelResources {
		if resource == Player || resource == Food {
			continue
		}
		droppedNb := 0
		for droppedNb < amount {
			game.Socket.SendCommand(network.SetObject, itemToString[resource])
			resp := game.awaitResponseToCommand()
			if resp {
				droppedNb++
			}
			game.updateFrequency()
		}
	}
}

func (game *Game) collectResourcesOnFailedLevelUp() {
	log.Println("Clearing resources after failed level up")
	game.Socket.SendCommand(network.LookAround, network.EmptyBody) // Ask server for a view map
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.updatePrioritiesFromViewMap() // Update the priorities using the viewmap
	pqTileItem := GetPriorityQueueItem(&game.Movement.TilesQueue, game.Coordinates.CoordsFromOrigin)
	if pqTileItem != nil { // Remove tile if it was in the queue
		game.collectTileResources(pqTileItem)
		RemoveFromPriorityQueue(&game.Movement.TilesQueue, pqTileItem.value)
		game.updatePriorityQueueAfterCollection()
		log.Println("Cleared resources after failed level up")
		return
	}
	log.Println("No resources to collect")
}

// startLevelUpHost starts the level up hosting process
func (game *Game) startLevelUpHost() {
	dropResources(game)
	startLevelUp(game, game.Level+1)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	log.Println("Starting level up process as host, target level", game.Level+1)
	game.Socket.SendCommand(network.LevelUp, network.EmptyBody)
	initialResponse := game.awaitResponseToCommand()
	if !initialResponse {
		levelUpFailed(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		game.updateFrequency()
		log.Println("Failed to start level up process as host, target level", game.Level+1)
		game.collectResourcesOnFailedLevelUp()
		return
	}
	finalResponse := game.awaitResponseToCommand()
	game.updateFrequency()
	if finalResponse {
		levelUpComplete(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		game.updateFrequency()
		log.Println("Successfully leveled up as host, new level", game.Level+1)
		game.Level += 1
		game.forkPlayer()
	} else {
		levelUpFailed(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		game.updateFrequency()
		log.Println("Failed to level up as host, target level", game.Level+1)
		game.collectResourcesOnFailedLevelUp()
	}
}

func awaitLevelUpLeechUpdate(game *Game, uuid string) bool {
	for {
		select {
		case value, ok := <-game.Socket.ResponseFeedbackChannel:
			if ok {
				return value
			}
		default:
		}
		message, err := popMessageFromQueue()
		if err == nil && message.uuid == uuid && (message.msgType == lvlUpComplete || message.msgType == lvlUpFailed) {
			return message.msgType == lvlUpComplete
		}
	}
}

// startLevelUpLeech starts the level up leeching process
func (game *Game) startLevelUpLeech(uuid string) {
	log.Println("Starting level up as leech, target level", game.Level+1)
	game.Socket.LastCommandType = network.LevelUp
	levelUpStatus := awaitLevelUpLeechUpdate(game, uuid)
	if levelUpStatus {
		log.Println("Successfully leveled up as leech, new level", game.Level+1)
		game.Level += 1
		game.forkPlayer()
	} else {
		log.Println("Failed to level up as leech, target level", game.Level+1)
	}
	game.MessageManager.waitingForLevelUp = false
	game.MessageManager.waitingForLevelUpLeech = false
}

// levelUpHostLoop starts a level up "lobby", and waits until there are enough players gathered
// or until the food reserves are too low to continue
func (game *Game) levelUpHostLoop() {
	targetLevel := game.Level + 1
	nbMissingPlayers := levelUpResources[game.Level][Player] - 1
	log.Println("Starting level up host : target level", targetLevel)
	for game.isLevelUpHostAvailable() {
		if nbMissingPlayers == 0 {
			game.startLevelUpHost()
			return
		}
		message, err := popMessageFromQueue()
		if err == nil {
			switch message.msgType {
			case announcePresence:
				nbMissingPlayers -= 1
			case announceDeparture:
				nbMissingPlayers += 1
			default:
			}
		}
		if nbMissingPlayers == 0 {
			continue
		}
		levelUpReadyMissingPlayers(game, targetLevel, nbMissingPlayers)
		_ = game.awaitResponseToCommand()
		game.updateFrequency()
	}
	log.Println("Not enough food to stanby and host, cancelling")
	cancelLevelUp(game, targetLevel)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
}

// levelUpHostLoop joins a level up "lobby", and waits until there are enough players gathered
// or until the food reserves are too low to continue
func (game *Game) levelUpLeechLoop(uuid string) {
	targetLevel := game.Level + 1
	log.Println("Joining level up : target level", targetLevel)
	announcePresenceLevelUp(game, targetLevel)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	for game.isLevelUpLeechAvailable() {
		message, err := popMessageFromQueue()
		if err == nil {
			switch message.msgType {
			case startLvlUp:
				game.startLevelUpLeech(uuid)
				return
			case cancelLvlUp:
				log.Println("Level up leech cancelled")
				return
			default:
			}
		}
	}
	log.Println("Not enough food to standby and leech, leaving")
	announceDepartureLevelUp(game, targetLevel)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
}

// forkPlayer if there are no slots left in the team, to ensure that the team can reach level 8 eventually
func (game *Game) forkPlayer() {
	if game.Level != 2 {
		return
	}
	game.Socket.SendCommand(network.GetUnusedSlots, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	if game.SlotsLeft == 0 && getFoodPriority(&game.FoodManager.FoodPriority) < 8 {
		log.Println("No slots left in team", game.TeamName, ", forking player")
		game.Socket.SendCommand(network.Fork, network.EmptyBody)
		_ = game.awaitResponseToCommand()
		game.updateFrequency()
	}
}
