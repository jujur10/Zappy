package ai

import (
	"fmt"
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
	Linemate:  "linemate",
	Deraumere: "deraumere",
	Sibur:     "sibur",
	Mendiane:  "mendiane",
	Phiras:    "phiras",
	Thystame:  "thystame",
}

// isLevelUpLeechAvailable checks if a hosted level up is available to leech from
func (game Game) isLevelUpLeechAvailable() bool {
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

// areLevelUpConditionsMet checks if all the level up conditions are met, i.e., enough food and
// Enough resources to host a level up OR a level up leeching
func (game Game) areLevelUpConditionsMet() bool {
	if game.FoodManager.FoodPriority >= 7 {
		return false
	}
	if game.isLevelUpLeechAvailable() {
		return true
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
		}
	}
}

// startLevelUpHost starts the level up hosting process
func (game Game) startLevelUpHost() {
	dropResources(&game)
	startLevelUp(game, game.Level+1)
	_ = game.awaitResponseToCommand()
	fmt.Println("Starting level up process as host, target level ", game.Level+1)
	game.Socket.SendCommand(network.LevelUp, network.EmptyBody)
	initialResponse := game.awaitResponseToCommand()
	if !initialResponse {
		levelUpFailed(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		log.Println("Failed to start level up process as host, target level ", game.Level+1)
		return
	}
	finalResponse := game.awaitResponseToCommand()
	if finalResponse {
		levelUpComplete(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		log.Println("Successfully leveled up as host, new level ", game.Level+1)
		game.Level += 1
		game.forkPlayer()
	} else {
		levelUpFailed(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		log.Println("Failed to level up as host, target level ", game.Level+1)
	}
}

// startLevelUpLeech starts the level up leeching process
func (game Game) startLevelUpLeech() {
	log.Println("Starting level up as leech, target level ", game.Level+1)
	game.Socket.SendCommand(network.LevelUp, network.EmptyBody)
	initialResponse := game.awaitResponseToCommand()
	if !initialResponse {
		log.Println("Failed to start level up as leech, target level ", game.Level+1)
		levelUpFailed(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		return
	}
	finalResponse := game.awaitResponseToCommand()
	if finalResponse {
		log.Println("Successfully leveled up as leech, new level ", game.Level+1)
		levelUpComplete(game, game.Level+1)
		_ = game.awaitResponseToCommand()
		game.Level += 1
		game.forkPlayer()
	} else {
		log.Println("Failed to level up as leech, target level ", game.Level+1)
		levelUpFailed(game, game.Level+1)
		_ = game.awaitResponseToCommand()
	}
}

// levelUpHostLoop starts a level up "lobby", and waits until there are enough players gathered
// or until the food reserves are too low to continue
func (game Game) levelUpHostLoop() {
	targetLevel := game.Level + 1
	nbMissingPlayers := levelUpResources[game.Level][Player] - 1
	log.Println("Starting level up host : target level ", targetLevel)
	for game.areLevelUpConditionsMet() {
		if nbMissingPlayers == 0 {
			game.startLevelUpHost()
			return
		}
		select {
		case message, ok := <-game.MessageManager.levelUpMessageChannel:
			if !ok {
				cancelLevelUp(game, targetLevel)
				_ = game.awaitResponseToCommand()
				return
			}
			switch message.msgType {
			case announcePresence:
				nbMissingPlayers -= 1
			case announceDeparture:
				nbMissingPlayers += 1
			default:
			}
		default:
		}
		if nbMissingPlayers == 0 {
			continue
		}
		levelUpReadyMissingPlayers(game, targetLevel, nbMissingPlayers)
		_ = game.awaitResponseToCommand()
	}
	log.Println("Not enough food to stanby and host, cancelling")
	cancelLevelUp(game, targetLevel)
	_ = game.awaitResponseToCommand()
}

// levelUpHostLoop joins a level up "lobby", and waits until there are enough players gathered
// or until the food reserves are too low to continue
func (game Game) levelUpLeechLoop() {
	targetLevel := game.Level + 1
	log.Println("Joining level up : target level ", targetLevel)
	announcePresenceLevelUp(game, targetLevel)
	_ = game.awaitResponseToCommand()
	for game.areLevelUpConditionsMet() {
		select {
		case message, ok := <-game.MessageManager.levelUpMessageChannel:
			if !ok {
				announceDepartureLevelUp(game, targetLevel)
				_ = game.awaitResponseToCommand()
				return
			}
			switch message.msgType {
			case startLvlUp:
				game.startLevelUpLeech()
				return
			case cancelLvlUp:
				log.Println("Level up leech cancelled")
				return
			default:
			}
		default:
		}
	}
	log.Println("Not enough food to standby and leech, leaving")
	announceDepartureLevelUp(game, targetLevel)
	_ = game.awaitResponseToCommand()
}

// forkPlayer if there are no slots left in the team, to ensure that the team can reach level 8 eventually
func (game Game) forkPlayer() {
	if game.Level != 2 {
		return
	}
	game.Socket.SendCommand(network.GetUnusedSlots, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	if game.SlotsLeft == 0 && game.FoodManager.FoodPriority < 8 {
		log.Println("No slots left in team ", game.TeamName, ", forking player")
		game.Socket.SendCommand(network.Fork, network.EmptyBody)
		_ = game.awaitResponseToCommand()
	}
}
