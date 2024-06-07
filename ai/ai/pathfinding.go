package ai

import (
	"container/heap"
	"math"
	"slices"
	"zappy_ai/network"
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

// updatePosition updates a position in a given direction, absolute modulo worldSize
func updatePosition(position, worldSize RelativeCoordinates, direction network.PlayerDirection) RelativeCoordinates {
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
func (game Game) computePath() []RelativeCoordinates {
	poppedDest := heap.Pop(&game.Movement.TilesQueue)
	if poppedDest == nil {
		return nil
	}
	destination := poppedDest.(RelativeCoordinates)
	origin := game.Coordinates.CoordsFromOrigin
	xDistance := Abs(destination[0] - origin[0])
	yDistance := Abs(destination[1] - origin[1])
	xSign := (destination[0] - origin[0]) / xDistance
	ySign := (destination[1] - origin[1]) / yDistance
	middlePoints := getMiddlePoints(origin, destination, game.Movement.TilesQueue)
	if len(middlePoints) == 0 {
		return computeBasicPath(origin, destination, xDistance, yDistance, xSign, ySign)
	}
	selectedMiddlePoints := computeOptimalPath(origin, destination, middlePoints) // Get the middle points to go through
	path := make([]RelativeCoordinates, 0)
	lastOrigin := origin
	for _, point := range selectedMiddlePoints { // Create the path linking all the points
		pathPart := computeBasicPath(lastOrigin, point, xDistance, yDistance, xSign, ySign)
		path = append(path, pathPart...)
	}
	return path
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
	for _, item := range game.Movement.TilesQueue {
		distance := ManhattanDistance(game.Coordinates.CoordsFromOrigin, item.value)
		positions[item] = Item{value: item.value, priority: max(0, item.originalPriority-distance),
			originalPriority: item.originalPriority, index: item.index}
	}
	for originalItem, newItem := range positions {
		game.Movement.TilesQueue.Update(originalItem, newItem.value, newItem.priority)
	}
}
