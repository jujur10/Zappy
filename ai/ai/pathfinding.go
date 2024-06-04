package ai

import "zappy_ai/network"

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
