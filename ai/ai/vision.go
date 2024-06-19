package ai

import (
	"container/heap"
	"fmt"
	"strings"
	"zappy_ai/network"
)

// parsedKeyToInvKey converts the strings to tileItems
var parsedKeyToInvKey = map[string]TileItem{"food": Food, "linemate": Linemate,
	"deraumere": Deraumere, "sibur": Sibur, "mendiane": Mendiane, "phiras": Phiras, "thystame": Thystame}

// CreateViewMap creates a ViewMap from a parsed double array of strings
func CreateViewMap(parsedList [][]string) (ViewMap, error) {
	size := 0
	counter := 1
	for len(parsedList) > size {
		size += counter
		counter += 2
	}
	if len(parsedList) != size {
		return nil, fmt.Errorf("invalid number of items in view map")
	}
	viewMap := make(ViewMap, len(parsedList))
	for idx, tile := range parsedList {
		viewMap[idx] = make([]TileItem, len(tile))
		for itemIdx, tileItem := range tile {
			tileItem = strings.ToLower(tileItem)
			var item TileItem
			if tileItem == "player" {
				item = Player
			} else if val, ok := parsedKeyToInvKey[tileItem]; ok {
				item = val
			} else {
				return nil, fmt.Errorf("invalid item in view map")
			}
			viewMap[idx][itemIdx] = item
		}
	}
	return viewMap, nil
}

// buildAbsoluteCoordsMap create a list containing the absolute coordinates of the tiles in the ViewMap
func buildAbsoluteCoordsMap(direction network.PlayerDirection, worldSize RelativeCoordinates,
	position RelativeCoordinates, viewMapLen int) []RelativeCoordinates {
	directionsVectors := map[network.PlayerDirection]RelativeCoordinates{Up: {0, 1},
		Down: {0, worldSize[1] - 1}, Right: {1, 0}, Left: {worldSize[0] - 1, 0}}
	forwardVector := directionsVectors[direction]
	leftVector := directionsVectors[(direction+3)%4]
	rightVector := directionsVectors[(direction+1)%4]
	absoluteMap := make([]RelativeCoordinates, 0)

	counter := 1
	size := 0
	depth := 0
	for size < viewMapLen {
		size += counter
		mid := (counter / 2) + 1
		for i := 1; i <= counter; i++ {
			frontAtDepth := RelativeCoordinates{position[0] + (forwardVector[0] * depth),
				position[1] + (forwardVector[1] * depth)}
			if i == mid {
				absoluteMap = append(absoluteMap, frontAtDepth)
			} else if i < mid {
				diff := mid - i
				point := RelativeCoordinates{frontAtDepth[0] + (leftVector[0] * diff), frontAtDepth[1] + (leftVector[1] * diff)}
				absoluteMap = append(absoluteMap, point)
			} else {
				diff := i - mid
				point := RelativeCoordinates{frontAtDepth[0] + (rightVector[0] * diff), frontAtDepth[1] + (rightVector[1] * diff)}
				absoluteMap = append(absoluteMap, point)
			}
		}
		counter += 2
		depth += 1
	}
	for idx := range absoluteMap {
		absoluteMap[idx][0] %= worldSize[0]
		absoluteMap[idx][1] %= worldSize[1]
	}
	return absoluteMap
}

// getTileUsefulResources returns a list containing all the useful resources of a tile
func getTileUsefulResources(totalResourcesNeeded Inventory, tile []TileItem) []TileItem {
	useful := make([]TileItem, 0)
	for _, resource := range tile {
		if resource == Player {
			continue
		}
		if totalResourcesNeeded[resource] > 0 || resource == Food {
			useful = append(useful, resource)
		}
	}
	return useful
}

// updatePrioritiesFromViewMap updates the PQueue using the generated ViewMap.
// It adds new useful tiles, updates already existing ones, and removes the useless ones
func (game *Game) updatePrioritiesFromViewMap() {
	absoluteMap := buildAbsoluteCoordsMap(game.Coordinates.Direction,
		game.Coordinates.WorldSize, game.Coordinates.CoordsFromOrigin, len(game.View))
	for tileIdx, viewedTile := range game.View {
		tilePrio := game.getTilePriority(viewedTile)
		pqIndex := game.Movement.TilesQueue.getPriorityQueueTileIndex(absoluteMap[tileIdx])
		if tilePrio > 0 && pqIndex == -1 {
			distance := ManhattanDistance(game.Coordinates.CoordsFromOrigin, absoluteMap[tileIdx])
			heap.Push(&game.Movement.TilesQueue, &Item{value: absoluteMap[tileIdx], originalPriority: tilePrio,
				priority: max(0, tilePrio-distance), action: ResourceCollection,
				usefulObjects: getTileUsefulResources(game.TotalResourcesRequired, viewedTile)})
		}
		if pqIndex != -1 {
			if tilePrio == 0 {
				heap.Remove(&game.Movement.TilesQueue, pqIndex)
				continue
			}
			game.Movement.TilesQueue[pqIndex].originalPriority = tilePrio
			game.Movement.TilesQueue[pqIndex].usefulObjects = getTileUsefulResources(game.TotalResourcesRequired, viewedTile)
			distance := ManhattanDistance(game.Coordinates.CoordsFromOrigin, absoluteMap[tileIdx])
			game.Movement.TilesQueue.Update(game.Movement.TilesQueue[pqIndex], absoluteMap[tileIdx], max(0, tilePrio-distance))
		}
	}
}
