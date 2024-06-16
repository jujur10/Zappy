package ai

import (
	"container/heap"
	"fmt"
	"log"
	"math"
	"slices"
	"zappy_ai/network"
)

type PlayerAction int

const (
	ResourceCollection PlayerAction = iota
	LevelUp
	LevelUpLeech
)

type graphPath struct {
	destination *graphNode
	length      int
}

type graphNode struct {
	pos           RelativeCoordinates
	weight        int
	paths         []graphPath
	isOrigin      bool
	isDestination bool
}

// The Path struct is a container for a destination item and the path of coords to reach it
type Path struct {
	destination Item
	path        []RelativeCoordinates
}

// updatePosition updates a position in a given direction, absolute modulo worldSize
func updatePosition(position, worldSize RelativeCoordinates, direction network.PlayerDirection) RelativeCoordinates {
	switch direction {
	case Down:
		position[1] = (position[1] + worldSize[1] - 1) % worldSize[1]
	case Up:
		position[1] = (position[1] + 1) % worldSize[1]
	case Left:
		position[0] = (position[0] + worldSize[0] - 1) % worldSize[0]
	case Right:
		position[0] = (position[0] + 1) % worldSize[0]
	default:
	}
	return position
}

// getMiddlePoints returns all the points contained in the priority queue,
// which are placed between the origin and destination
func getMiddlePoints(origin, destination RelativeCoordinates, pqueue PriorityQueue) []*Item {
	possiblePoints := make([]*Item, 0)
	for _, element := range pqueue {
		if element.value[0] >= min(origin[0], destination[0]) && // Check if the point is in the moving range
			element.value[0] <= max(origin[0], destination[0]) &&
			element.value[1] >= min(origin[1], destination[1]) &&
			element.value[1] <= max(origin[1], destination[1]) {
			possiblePoints = append(possiblePoints, element)
		}
	}

	return possiblePoints
}

// createGraph creates a graph using the origin, destination and middle points for the Dijkstra algorithm
func createGraph(origin, destination RelativeCoordinates, middlePoints []*Item) []graphNode {
	graph := make([]graphNode, 1)
	graph[0] = graphNode{isOrigin: true, isDestination: false, weight: 0, pos: origin} // Add origin
	for _, point := range middlePoints {                                               // Create graph nodes
		graph = append(graph,
			graphNode{isOrigin: false, isDestination: false, weight: point.originalPriority, pos: point.value})
	}
	// Add destination node
	graph = append(graph, graphNode{isOrigin: false, isDestination: true, weight: 0, pos: destination})
	graphLastIdx := len(graph) - 1

	for origIdx := range graph { // Add links between nodes
		links := make([]graphPath, 0)
		for destIdx := range graph {
			if graph[origIdx].pos == graph[destIdx].pos || // Avoid loops and connecting origin to destination
				(origIdx == 0 && destIdx == graphLastIdx) ||
				(destIdx == 0 && origIdx == graphLastIdx) {
				continue
			}
			links = append(links, graphPath{destination: &graph[destIdx],
				length: ManhattanDistance(graph[origIdx].pos, graph[destIdx].pos)})
		}
		graph[origIdx].paths = links
	}
	return graph
}

// getQueueItemPtr returns the pointer to the item in the queue containing node
func getQueueItemPtr(queue graphPriorityQueue, node *graphNode) *graphItem {
	for _, element := range queue {
		if element.node == node {
			return element
		}
	}
	return nil
}

// computeOptimalPath returns the path from origin to destination while going through as many middle points as possible.
// It uses a modified Dijkstra Algorithm
func computeOptimalPath(origin, destination RelativeCoordinates, middlePoints []*Item) []RelativeCoordinates {
	graph := createGraph(origin, destination, middlePoints)
	queue := make(graphPriorityQueue, 1) // Create priority queue
	queue[0] = &graphItem{node: &graph[0], priority: 0, index: 0}

	distanceMap := make(map[RelativeCoordinates]int)           // Create distances map
	weightsMap := make(map[RelativeCoordinates]int)            // Create weights map
	predecessorsMap := make(map[RelativeCoordinates]graphNode) // Create predecessors map

	for nodeIdx := range graph { // Initialize the priority queue with all the nodes as 'unreachable'
		if graph[nodeIdx].isOrigin == false {
			distanceMap[graph[nodeIdx].pos] = math.MaxInt
			weightsMap[graph[nodeIdx].pos] = 0
			heap.Push(&queue, &graphItem{node: &graph[nodeIdx], priority: math.MaxInt})
		}
	}

	for queue.Len() > 0 { // Until all the paths are processed
		bestNode := heap.Pop(&queue).(*graphItem)  // Pop the nearest node
		for _, link := range bestNode.node.paths { // Iterate over all links
			dist := distanceMap[bestNode.node.pos] + link.length // Compute next values
			weight := weightsMap[bestNode.node.pos] + link.destination.weight

			if dist < distanceMap[link.destination.pos] || // If the path is shorter or the weight is higher
				(dist <= distanceMap[link.destination.pos] && weight > weightsMap[link.destination.pos]) {
				predecessorsMap[link.destination.pos] = *bestNode.node // Update nodes values
				distanceMap[link.destination.pos] = dist
				weightsMap[link.destination.pos] = weight
				queue.Update(getQueueItemPtr(queue, link.destination), link.destination, dist)
			}
		}
	}

	out := make([]RelativeCoordinates, 0)
	nodeKey := destination
	for nodeKey != origin { // Get the path
		out = append(out, nodeKey)
		nodeKey = predecessorsMap[nodeKey].pos
	}
	out = out[1:]
	slices.Reverse(out) // Reverse the path

	return out
}

// computeBasicPath returns a basic path from origin to destination
func computeBasicPath(origin, destination RelativeCoordinates,
	xDistance, yDistance, xSign, ySign int) []RelativeCoordinates {
	path := make([]RelativeCoordinates, xDistance+yDistance)
	for i := range yDistance {
		path[i] = RelativeCoordinates{origin[0], origin[1] + ((i + 1) * ySign)}
	}
	for i := range xDistance {
		path[i+yDistance] = RelativeCoordinates{origin[0] + ((i + 1) * xSign), destination[1]}
	}
	return path
}

// computePath to the tile with the highest priority and passing through as many prioritized tiles as possible
func (game Game) computePath() (Path, error) {
	poppedDest := heap.Pop(&game.Movement.TilesQueue)
	if poppedDest == nil {
		return Path{}, fmt.Errorf("no more items in priority queue")
	}
	destinationItem := poppedDest.(*Item)
	destination := destinationItem.value
	origin := game.Coordinates.CoordsFromOrigin
	worldSize := game.Coordinates.WorldSize

	xDistance := Abs(destination[0] - origin[0])
	xDistanceWrap := Abs(destination[0] - origin[0] + worldSize[0])
	xSign := (destination[0] - origin[0]) / xDistance
	if xDistanceWrap < xDistance { // If wrapping around in X is shorter
		xDistance = xDistanceWrap
		xSign = (destination[0] - origin[0] + worldSize[0]) / xDistance
	}

	yDistance := Abs(destination[1] - origin[1])
	yDistanceWrap := Abs(destination[1] - origin[1] + worldSize[1])
	ySign := (destination[1] - origin[1]) / yDistance
	if yDistanceWrap < yDistance { // If wrapping around in Y is shorter
		yDistance = yDistanceWrap
		ySign = (destination[1] - origin[1] + worldSize[1]) / yDistance
	}

	middlePoints := getMiddlePoints(origin, destination, game.Movement.TilesQueue)
	if len(middlePoints) == 0 { // If there are no intermediate points to go through, make a basic path
		return Path{destination: *destinationItem,
			path: computeBasicPath(origin, destination, xDistance, yDistance, xSign, ySign)}, nil
	} // Else generate the most optimal path
	selectedMiddlePoints := computeOptimalPath(origin, destination, middlePoints) // Get the middle points to go through
	path := make([]RelativeCoordinates, 0)
	lastOrigin := origin
	for _, point := range selectedMiddlePoints { // Create the path linking all the points
		pathPart := computeBasicPath(lastOrigin, point, xDistance, yDistance, xSign, ySign)
		path = append(path, pathPart...)
	}
	return Path{destination: *destinationItem, path: path}, nil
}

// movePlayerForward moves the player and updates its position, and updates the movement priority queue
func (game Game) movePlayerForward() {
	game.Socket.SendCommand(network.GoForward, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.Coordinates.CoordsFromOrigin =
		updatePosition(game.Coordinates.CoordsFromOrigin, game.Coordinates.WorldSize, game.Coordinates.Direction)
	game.updateMovementQueueOnMove()
}

// turnLeft turns the player 90° left, and updates its direction
func (game Game) turnLeft() {
	game.Socket.SendCommand(network.RotateLeft, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.Coordinates.Direction += network.Left % 4 // (N + 3) % 4 = N - 1
}

// turnRight turns the player 90° right, and updates its direction
func (game Game) turnRight() {
	game.Socket.SendCommand(network.RotateRight, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.Coordinates.Direction += network.Right % 4 // (N + 1) % 4
}

// updateMovementQueueOnMove updates the priority of the items in the priority queue
func (game Game) updateMovementQueueOnMove() {
	positions := make(map[*Item]Item)
	for _, item := range game.Movement.TilesQueue {
		switch item.action {
		case LevelUp:
			positions[item] = Item{value: game.Coordinates.CoordsFromOrigin, priority: levelUpPriority,
				originalPriority: item.originalPriority, index: item.index, usefulObjects: item.usefulObjects}
		case LevelUpLeech:
			positions[item] = Item{value: item.value, priority: leechLevelUpPriority,
				originalPriority: item.originalPriority, index: item.index, usefulObjects: item.usefulObjects}
		case ResourceCollection:
			distance := ManhattanDistance(game.Coordinates.CoordsFromOrigin, item.value)
			positions[item] = Item{value: item.value, priority: max(0, item.originalPriority-distance),
				originalPriority: item.originalPriority, index: item.index, usefulObjects: item.usefulObjects}
		default:
		}
	}
	for originalItem, newItem := range positions {
		game.Movement.TilesQueue.Update(originalItem, newItem.value, newItem.priority)
	}
}

// getTileDirection returns the direction in which the given tile is from the current position
// Returns -1 in case of error
func getTileDirection(pos RelativeCoordinates, tile RelativeCoordinates) network.PlayerDirection {
	deltaX := tile[0] - pos[0]
	deltaY := tile[1] - pos[1]
	if deltaY == 0 { // Horizontal movement
		if deltaX > 0 {
			return network.Right
		}
		if deltaX < 0 {
			return network.Left
		}
	}
	if deltaX == 0 {
		if deltaY > 0 {
			return network.Up
		}
		if deltaY < 0 {
			return network.Down
		}
	}
	return -1
}

// moveToTile moves the player to a given adjacent tile
func (game Game) moveToTile(tile RelativeCoordinates) {
	if ManhattanDistance(game.Coordinates.CoordsFromOrigin, tile) != 1 {
		log.Println("Error! Cannot move to non adjacent tile ", tile, ", position is ", game.Coordinates.CoordsFromOrigin)
		return
	}
	direction := getTileDirection(game.Coordinates.CoordsFromOrigin, tile)
	if direction == -1 {
		log.Println("Error targeted tile: Invalid direction !")
		return
	}
	leftOffset := direction - game.Coordinates.Direction
	rightOffset := game.Coordinates.Direction - direction
	if rightOffset > leftOffset {
		for range leftOffset {
			game.turnLeft()
		}
	} else {
		for range rightOffset {
			game.turnRight()
		}
	}
	game.movePlayerForward()
}

// followPath follows the given path while collecting all useful resources on the visited tiles
func (game Game) followPath(path Path) {
	for _, tile := range path.path {
		game.moveToTile(tile)
		game.Socket.SendCommand(network.LookAround, network.EmptyBody) // Ask server for a view map
		_ = game.awaitResponseToCommand()
		game.updatePrioritiesFromViewMap() // Update the priorities using the viewmap
		pqTileIndex := game.Movement.TilesQueue.getPriorityQueueTileIndex(tile)
		if pqTileIndex != -1 { // Remove tile if it was in the queue
			game.collectTileResources(pqTileIndex)
			heap.Remove(&game.Movement.TilesQueue, pqTileIndex)
			game.updatePriorityQueueAfterCollection()
		}
		if game.Movement.TilesQueue[0].originalPriority > path.destination.originalPriority &&
			game.Movement.TilesQueue[0].action == ResourceCollection {
			return // Check for a higher priority task
		}
	}
}
