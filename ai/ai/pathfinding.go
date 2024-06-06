package ai

import (
	"container/heap"
	"zappy_ai/network"
)

// updatePosition updates a position in a given direction, absolute modulo worldSize
func updatePosition(position RelativeCoordinates, worldSize RelativeCoordinates,
	direction network.PlayerDirection) RelativeCoordinates {
	switch direction {
	case Down:
		position[1] -= 1
		if position[1] < 0 {
			position[1] = worldSize[1] - 1
		}
	case Up:
		position[1] = (position[1] + 1) % worldSize[1]
	case Left:
		position[0] = position[0] - 1
		if position[0] < 0 {
			position[0] = worldSize[0] - 1
		}
	case Right:
		position[0] = (position[0] + 1) % worldSize[0]
	default:
	}
	return position
}

func getMiddlePoints(origin, destination RelativeCoordinates, pqueue PriorityQueue) []RelativeCoordinates {
	var possiblePoints []RelativeCoordinates
	for _, element := range pqueue {
		if element.value[0] >= min(origin[0], destination[0]) && // Check if the point is in the moving range
			element.value[0] <= max(origin[0], destination[0]) &&
			element.value[1] >= min(origin[1], destination[1]) &&
			element.value[1] <= max(origin[1], destination[1]) {
			possiblePoints = append(possiblePoints, element.value)
		}
	}

	return possiblePoints
}

func getNextPoints(current, destination RelativeCoordinates, xSign, ySign int) []RelativeCoordinates {
	nextPoints := make([]RelativeCoordinates, 0)
	if current[0] != destination[0] {
		nextPoints = append(nextPoints, RelativeCoordinates{current[0] + xSign, current[1]})
	}
	if current[1] != destination[1] {
		nextPoints = append(nextPoints, RelativeCoordinates{current[0], current[1] + ySign})
	}
	return nextPoints
}

// computeOptimalPath returns the path from origin to destination while going through as many middle points as possible
func computeOptimalPath(origin, destination RelativeCoordinates, middlePoints []RelativeCoordinates,
	xSign int, ySign int) []RelativeCoordinates {
	distanceMap := make(map[RelativeCoordinates]int)
	pathMap := make(map[RelativeCoordinates][]RelativeCoordinates)

	distanceMap[origin] = 0
	pathMap[origin] = []RelativeCoordinates{origin}
	queue := make(PriorityQueue, 1)
	queue[0] = &Item{value: origin, priority: 0}
	heap.Init(&queue)
	for queue.Len() > 0 {
		current := heap.Pop(&queue).(*Item).value

		if current == destination {
			return pathMap[current]
		}
		nextPoints := getNextPoints(current, destination, xSign, ySign)
		for _, next := range nextPoints {
			newDistance := distanceMap[current] + 1
			newPath := append(pathMap[current], next)

			priority := -newDistance
			if network.IsInArray(next, middlePoints) {
				priority -= 1
			}

			if _, ok := distanceMap[next]; !ok || newDistance < distanceMap[next] {
				distanceMap[next] = newDistance
				pathMap[next] = newPath
				heap.Push(&queue, &Item{value: next, priority: priority})
			}
		}
	}
	return nil
}

// computeBasicPath returns a basic path from origin to destination
func computeBasicPath(origin, destination RelativeCoordinates,
	xDistance, yDistance, xSign, ySign int) []RelativeCoordinates {
	path := make([]RelativeCoordinates, xDistance+yDistance)
	for i := range yDistance {
		path = append(path, RelativeCoordinates{origin[0], origin[1] + ((i + 1) * ySign)})
	}
	for i := range xDistance {
		path = append(path, RelativeCoordinates{origin[0] + ((i + 1) * xSign), destination[1]})
	}
	return path
}

// computePath to the tile with the highest priority and passing through as many prioritized tiles as possible
func (game Game) computePath() []RelativeCoordinates {
	poppedDest := heap.Pop(&game.MovementQueue)
	if poppedDest == nil {
		return nil
	}
	destination := poppedDest.(RelativeCoordinates)
	origin := game.Coordinates.CoordsFromOrigin
	xDistance := Abs(destination[0] - origin[0])
	yDistance := Abs(destination[1] - origin[1])
	xSign := (destination[0] - origin[0]) / xDistance
	ySign := (destination[1] - origin[1]) / yDistance
	middlePoints := getMiddlePoints(origin, destination, game.MovementQueue)
	if len(middlePoints) == 0 {
		return computeBasicPath(origin, destination, xDistance, yDistance, xSign, ySign)
	}
	return computeOptimalPath(origin, destination, middlePoints, xSign, ySign)
}

// movePlayer moves the player and updates its position, and updates the movement priority queue
func (game Game) movePlayer() {
	game.Coordinates.CoordsFromOrigin =
		updatePosition(game.Coordinates.CoordsFromOrigin, game.Coordinates.WorldSize, game.Coordinates.Direction)
	game.updateMovementQueueOnMove()
}

// updateMovementQueueOnMove updates the priority of the items in the priority queue
func (game Game) updateMovementQueueOnMove() {
	positions := make(map[*Item]Item)
	for _, item := range game.MovementQueue {
		distance := ManhattanDistance(game.Coordinates.CoordsFromOrigin, item.value)
		positions[item] = Item{value: item.value, priority: max(0, item.originalPriority-distance),
			originalPriority: item.originalPriority, index: item.index}
	}
	for originalItem, newItem := range positions {
		game.MovementQueue.Update(originalItem, newItem.value, newItem.priority)
	}
}
