package ai

import (
	"container/heap"
	"log"
	"zappy_ai/network"
)

// levelUpResources is a map of TileItem -> ints maps containing the necessary resources for level ups
// This is a pre-initialized placeholder for creating the game struct
var levelUpResources = map[int]Inventory{
	1: {Player: 1, Linemate: 1, Deraumere: 0, Sibur: 0, Mendiane: 0, Phiras: 0, Thystame: 0},
	2: {Player: 2, Linemate: 1, Deraumere: 1, Sibur: 1, Mendiane: 0, Phiras: 0, Thystame: 0},
	3: {Player: 2, Linemate: 2, Deraumere: 0, Sibur: 1, Mendiane: 0, Phiras: 2, Thystame: 0},
	4: {Player: 4, Linemate: 1, Deraumere: 1, Sibur: 2, Mendiane: 0, Phiras: 1, Thystame: 0},
	5: {Player: 4, Linemate: 1, Deraumere: 2, Sibur: 1, Mendiane: 3, Phiras: 0, Thystame: 0},
	6: {Player: 6, Linemate: 1, Deraumere: 2, Sibur: 3, Mendiane: 0, Phiras: 1, Thystame: 0},
	7: {Player: 6, Linemate: 2, Deraumere: 2, Sibur: 2, Mendiane: 2, Phiras: 2, Thystame: 1},
}

// totalResourcesRequired is an Inventory containing the total resources left to collect to reach level 8
// This is a pre-initialized placeholder for creating the game struct
var totalResourcesRequired = Inventory{Linemate: 9, Deraumere: 8, Sibur: 10, Mendiane: 5, Phiras: 6, Thystame: 1}

// levelUpPriority is the priority of starting the level up process
const levelUpPriority = 8

// leechLevelUpPriority is the priority for leeching of a level up process from another player
const leechLevelUpPriority = 9

// resourceCollected update the resource tables or the food goroutine when the player collects a resource
func (game *Game) resourceCollected(item TileItem) {
	if item == Player {
		return
	}
	if item == Food {
		game.FoodManager.InputFoodChannel <- 1
		return
	}
	for i := game.Level; i < 8; i++ {
		if game.LevelUpResources[i][item] > 0 {
			game.LevelUpResources[i][item]--
			break
		}
	}
	if game.TotalResourcesRequired[item] > 0 {
		game.TotalResourcesRequired[item]--
	}
}

// removeLevelUpResources removes the resources required for the previous level up
// removeLevelUpResources also updates the total resources required to reach level 8
func (game *Game) removeLevelUpResources() {
	for i := 1; i < game.Level; i++ {
		for key, value := range game.LevelUpResources[i] {
			if key != Player {
				game.TotalResourcesRequired[key] -= value
				game.LevelUpResources[i][key] = 0
			}
		}
	}
}

// isResourceRequired checks if the resource is going to be needed at some point in the future.
// isResourceRequired always return false for a Player and true for Food
func (game *Game) isResourceRequired(item TileItem) bool {
	if item == Player {
		return false
	}
	if item == Food {
		return true
	}
	return game.TotalResourcesRequired[item] > 0
}

// getResourcePriority returns the priority for the item based on the resources needed for level-up(s)
func (game *Game) getResourcePriority(item TileItem) int {
	if item == Player {
		return 0
	}
	if item == Food {
		return getFoodPriority(&game.FoodManager.FoodPriority)
	}
	priority := 8
	for i := game.Level; i < 8; i++ {
		if game.LevelUpResources[i][item] > 0 {
			return priority
		}
		priority--
	}
	return 0
}

// getTilePriority returns the max priority of the items on this tile
func (game *Game) getTilePriority(tile []TileItem) int {
	prio := 0
	for _, item := range tile {
		if item == Player {
			return 0
		}
		itemPrio := game.getResourcePriority(item)
		prio = max(prio, itemPrio)
	}
	return prio
}

// getCurrentTilePriority returns the max priority of the items on the current tile
func (game *Game) getCurrentTilePriority(tile []TileItem) int {
	prio := 0
	nbPlayers := 0
	for _, item := range tile {
		if item == Player {
			nbPlayers++
		}
		itemPrio := game.getResourcePriority(item)
		prio = max(prio, itemPrio)
	}
	if nbPlayers > 1 {
		return 0
	}
	return prio
}

// getLevelUpPriority returns the priority at which the level up process should be started
func (game *Game) getLevelUpPriority() int {
	if getFoodPriority(&game.FoodManager.FoodPriority) > 6 {
		return 0
	}
	if game.isLevelUpLeechAvailable() {
		return leechLevelUpPriority
	}
	neededResourcesSum := 0
	for key, value := range game.LevelUpResources[game.Level] {
		if key == Player || key == Food {
			continue
		}
		neededResourcesSum += value
	}
	if neededResourcesSum > 0 {
		return 0
	}
	return levelUpPriority
}

// Abs returns the absolute value of an integer
func Abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

// ManhattanDistance returns the Manhattan / taxicab distance between two points
func ManhattanDistance(pos1 RelativeCoordinates, pos2 RelativeCoordinates) int {
	return Abs(pos2[0]-pos1[0]) + Abs(pos2[1]-pos1[1])
}

// collectTileResources collects all the resources of a tile that are useful to the player
func (game *Game) collectTileResources(pqTileIndex int) {
	item := game.Movement.TilesQueue[pqTileIndex]
	log.Println("Collect tile resources function entered, item", *item)
	nbPlayers := 0
	for _, resource := range (*item).usefulObjects {
		if resource == Player {
			nbPlayers++
		}
	}
	if nbPlayers > 1 {
		return
	}
	for _, resource := range (*item).usefulObjects {
		if game.isResourceRequired(resource) {
			log.Println("Trying to collect ressource", resource)
			game.Socket.SendCommand(network.TakeObject, itemToString[resource])
			status := game.awaitResponseToCommand()
			game.updateFrequency()
			if status {
				game.resourceCollected(resource)
			}
		}
	}
}

// updatePriorityQueueAfterCollection updates all the priorities in the PQueue after that a tile was harvested
// If a tile is no longer useful, it is removed from the queue
func (game *Game) updatePriorityQueueAfterCollection() {
	positions := make(map[*Item]Item)
	for _, item := range game.Movement.TilesQueue {
		if item.action == ResourceCollection {
			distance := ManhattanDistance(game.Coordinates.CoordsFromOrigin, item.value)
			usefulObjs := make([]TileItem, 0)
			for _, obj := range item.usefulObjects {
				if game.isResourceRequired(obj) {
					usefulObjs = append(usefulObjs, obj)
				}
			}
			newOriginalPrio := game.getTilePriority(usefulObjs)
			positions[item] = Item{value: item.value, priority: max(0, newOriginalPrio-distance),
				originalPriority: newOriginalPrio, index: item.index, usefulObjects: usefulObjs}
		}
	}
	for originalItem, newItem := range positions {
		if newItem.originalPriority == 0 {
			heap.Remove(&game.Movement.TilesQueue, originalItem.index)
		} else {
			game.Movement.TilesQueue.Update(originalItem, newItem.value, newItem.priority)
		}
	}
}
