package ai

import (
	"container/heap"
	"fmt"
	"log"
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
func getMiddlePoints(origin, destination, worldSize RelativeCoordinates, pqueue PriorityQueue) []*Item {
	possiblePoints := make([]*Item, 0)

	nonWrappingX := Abs(destination[0] - origin[0])
	nonWrappingY := Abs(destination[1] - origin[1])
	wrappingX := 0
	if origin[0] > destination[0] {
		wrappingX = (destination[0] - origin[0] + worldSize[0]) % worldSize[0]
	} else {
		wrappingX = (origin[0] - destination[0] + worldSize[0]) % worldSize[0]
	}
	wrappingY := 0
	if origin[1] > destination[1] {
		wrappingY = (destination[1] - origin[1] + worldSize[1]) % worldSize[1]
	} else {
		wrappingY = (origin[1] - destination[1] + worldSize[1]) % worldSize[1]
	}

	xMin := min(origin[0], destination[0])
	xMax := max(origin[0], destination[0])
	yMin := min(origin[1], destination[1])
	yMax := max(origin[1], destination[1])
	xWrapping := wrappingX < nonWrappingX
	yWrapping := wrappingY < nonWrappingY

	for _, element := range pqueue {
		if element.value == origin {
			log.Println("Origin is in Middle Points")
			continue
		}
		if element.value == destination {
			log.Println("Destination is in Middle Points")
		}
		if element.value[0] >= worldSize[0] || element.value[1] >= worldSize[1] {
			continue
		}
		xOk := false
		if !xWrapping {
			xOk = element.value[0] >= xMin && element.value[0] <= xMax
		} else {
			xOk = element.value[0] <= xMin || element.value[0] >= xMax
		}

		yOk := false
		if !yWrapping {
			yOk = element.value[1] >= yMin && element.value[1] <= yMax
		} else {
			yOk = element.value[1] <= yMin || element.value[1] >= yMax
		}

		if xOk && yOk { // Check if the point is in the moving range
			possiblePoints = append(possiblePoints, element)
		}

	}

	return possiblePoints
}

// createGraph creates a graph using the origin, destination and middle points for the Dijkstra algorithm
func createGraph(origin, destination, worldSize RelativeCoordinates, middlePoints []*Item) []graphNode {
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
				length: ManhattanDistance(graph[origIdx].pos, graph[destIdx].pos, worldSize)})
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
func computeOptimalPath(origin, destination, worldSize RelativeCoordinates,
	middlePoints []*Item) []RelativeCoordinates {
	graph := createGraph(origin, destination, worldSize, middlePoints)
	queue := make(graphPriorityQueue, 1) // Create priority queue
	queue[0] = &graphItem{node: &graph[0], priority: 0, index: 0}
	heap.Init(&queue)

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
				itemPtr := getQueueItemPtr(queue, link.destination)
				queue.Update(itemPtr, link.destination, dist)
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
func computeBasicPath(origin, destination, worldSize RelativeCoordinates,
	xDistance, yDistance, xSign, ySign int) []RelativeCoordinates {
	path := make([]RelativeCoordinates, xDistance+yDistance)
	if worldSize[0] == 0 || worldSize[1] == 0 {
		log.Fatal("World size cannot be null")
	}
	for i := range yDistance {
		path[i] = RelativeCoordinates{origin[0], (origin[1] + worldSize[1] + ((i + 1) * ySign)) % worldSize[1]}
	}
	for i := range xDistance {
		path[i+yDistance] = RelativeCoordinates{(origin[0] + worldSize[0] + ((i + 1) * xSign)) % worldSize[0], destination[1]}
	}
	return path
}

// computePath to the tile with the highest priority and passing through as many prioritized tiles as possible
func (game *Game) computePath() (Path, error) {
	poppedDest := PopFromPriorityQueue(&game.Movement.TilesQueue)
	if poppedDest == nil {
		return Path{}, fmt.Errorf("no more items in priority queue")
	}
	destination := poppedDest.value
	origin := game.Coordinates.CoordsFromOrigin
	worldSize := game.Coordinates.WorldSize
	log.Println("Creating new path to", destination)

	xDistance := Abs(destination[0] - origin[0])
	xDistanceWrap := 0
	if origin[0] > destination[0] {
		xDistanceWrap = (destination[0] - origin[0] + worldSize[0]) % worldSize[0]
	} else {
		xDistanceWrap = (origin[0] - destination[0] + worldSize[0]) % worldSize[0]
	}
	xSign := 1
	if xDistance != 0 {
		xSign = (destination[0] - origin[0]) / xDistance
	}
	if xDistanceWrap < xDistance { // If wrapping around in X is shorter
		xDistance = xDistanceWrap
		xSign = 1
		if xDistance != 0 {
			xSign = ((destination[0] - origin[0] + worldSize[0]) % worldSize[0]) / xDistance
		}
	}

	log.Println("Computing path xDistance", xDistance, "xDistanceWrap", xDistanceWrap, "xSign", xSign)

	yDistance := Abs(destination[1] - origin[1])
	yDistanceWrap := 0
	if origin[1] > destination[1] {
		yDistanceWrap = (destination[1] - origin[1] + worldSize[1]) % worldSize[1]
	} else {
		yDistanceWrap = (origin[1] - destination[1] + worldSize[1]) % worldSize[1]
	}
	ySign := 1
	if yDistance != 0 {
		ySign = (destination[1] - origin[1]) / yDistance
	}
	if yDistanceWrap < yDistance { // If wrapping around in Y is shorter
		yDistance = yDistanceWrap
		ySign = 1
		if yDistance != 0 {
			ySign = ((destination[1] - origin[1] + worldSize[1]) % worldSize[1]) / yDistance
		}
	}
	log.Println("Computing path yDistance", yDistance, "yDistanceWrap", yDistanceWrap, "ySign", ySign)

	middlePoints := getMiddlePoints(origin, destination, worldSize, game.Movement.TilesQueue)
	if len(middlePoints) == 0 { // If there are no intermediate points to go through, make a basic path
		return Path{destination: *poppedDest,
			path: computeBasicPath(origin, destination, worldSize, xDistance, yDistance, xSign, ySign)}, nil
	} // Else generate the most optimal path
	// Get the middle points to go through
	selectedMiddlePoints := computeOptimalPath(origin, destination, worldSize, middlePoints)
	path := make([]RelativeCoordinates, 0)
	lastOrigin := origin
	for _, point := range selectedMiddlePoints { // Create the path linking all the points
		pathPart := computeBasicPath(lastOrigin, point, worldSize, xDistance, yDistance, xSign, ySign)
		path = append(path, pathPart...)
	}
	return Path{destination: *poppedDest, path: path}, nil
}

// movePlayerForward moves the player and updates its position, and updates the movement priority queue
func (game *Game) movePlayerForward() {
	game.Socket.SendCommand(network.GoForward, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.Coordinates.CoordsFromOrigin =
		updatePosition(game.Coordinates.CoordsFromOrigin, game.Coordinates.WorldSize, game.Coordinates.Direction)
	game.updateMovementQueueOnMove()
}

// turnLeft turns the player 90° left, and updates its direction
func (game *Game) turnLeft() {
	game.Socket.SendCommand(network.RotateLeft, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.Coordinates.Direction = (game.Coordinates.Direction + network.Left) % 4 // (N + 3) % 4 = N - 1
}

// turnRight turns the player 90° right, and updates its direction
func (game *Game) turnRight() {
	game.Socket.SendCommand(network.RotateRight, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.Coordinates.Direction = (game.Coordinates.Direction + network.Right) % 4 // (N + 1) % 4
}

// updateMovementQueueOnMove updates the priority of the items in the priority queue
func (game *Game) updateMovementQueueOnMove() {
	positions := make([]Item, 0)
	for _, item := range game.Movement.TilesQueue {
		distance := ManhattanDistance(game.Coordinates.CoordsFromOrigin, item.value, game.Coordinates.WorldSize)
		positions = append(positions, Item{value: item.value, priority: max(0, item.originalPriority-distance),
			originalPriority: item.originalPriority, index: item.index, usefulObjects: item.usefulObjects})
	}
	for _, newItem := range positions {
		UpdatePriorityQueue(&game.Movement.TilesQueue, newItem.value, newItem.priority)
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
func (game *Game) moveToTile(tile RelativeCoordinates) {
	if ManhattanDistance(game.Coordinates.CoordsFromOrigin, tile, game.Coordinates.WorldSize) != 1 {
		log.Println("Error! Cannot move to non adjacent tile ", tile, ", position is ", game.Coordinates.CoordsFromOrigin)
		return
	}
	direction := getTileDirection(game.Coordinates.CoordsFromOrigin, tile)
	if direction == -1 {
		log.Println("Error targeted tile: Invalid direction !")
		return
	}
	rightOffset := (direction + (4 - game.Coordinates.Direction)) % 4
	leftOffset := (game.Coordinates.Direction + (4 - direction)) % 4
	log.Println("Trying to move from tile", game.Coordinates.CoordsFromOrigin, "to tile", tile, "Dest direction", direction, "left offset", leftOffset, "right offset", rightOffset)
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
func (game *Game) followPath(path Path) Path {
	if len(path.path) == 0 {
		return Path{path.destination, nil}
	}
	tile := path.path[0]
	path.path = path.path[1:]
	log.Println("Path next tile", tile, "new path", path.path)
	game.moveToTile(tile)
	game.Socket.SendCommand(network.LookAround, network.EmptyBody) // Ask server for a view map
	_ = game.awaitResponseToCommand()
	game.updateFrequency()
	game.updatePrioritiesFromViewMap() // Update the priorities using the viewmap
	pqTileItem := GetPriorityQueueItem(&game.Movement.TilesQueue, tile)
	log.Println("Follow path PQ Index for tile", tile, pqTileItem)
	if pqTileItem != nil { // Remove tile if it was in the queue
		game.collectTileResources(pqTileItem)
		log.Println("Trying to remove tile", tile, "from PQueue at index", pqTileItem.index)
		for _, elem := range game.Movement.TilesQueue {
			log.Println("#>", *elem)
		}
		RemoveFromPriorityQueue(&game.Movement.TilesQueue, pqTileItem.value)
		game.updatePriorityQueueAfterCollection()
	}
	if len(game.Movement.TilesQueue) > 0 &&
		game.Movement.TilesQueue[0].originalPriority > path.destination.originalPriority {
		newPath, err := game.computePath() // Check for a higher priority task
		if err != nil {
			return path
		}
		PushToPriorityQueue(&game.Movement.TilesQueue, path.destination)
		return newPath
	}
	return path
}

func (game *Game) followMessageDirection(direction network.EventDirection) {
	log.Println("Following player message direction:", direction)
	worldSize := game.Coordinates.WorldSize
	pos := game.Coordinates.CoordsFromOrigin
	directionsVectors := map[network.PlayerDirection]RelativeCoordinates{Up: {0, 1},
		Down: {0, worldSize[1] - 1}, Right: {1, 0}, Left: {worldSize[0] - 1, 0}}
	frontVec := directionsVectors[game.Coordinates.Direction]
	leftVec := directionsVectors[(game.Coordinates.Direction+3)%4]
	rearVec := directionsVectors[(game.Coordinates.Direction+2)%4]
	rightVec := directionsVectors[(game.Coordinates.Direction+1)%4]
	pathToFollow := make([]RelativeCoordinates, 0)
	switch direction {
	case 0:
		return
	case 1:
		frontPos := RelativeCoordinates{frontVec[0] + pos[0], frontVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, frontPos)
	case 2:
		frontPos := RelativeCoordinates{frontVec[0] + pos[0], frontVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, frontPos)
		frontLeftPost := RelativeCoordinates{frontPos[0] + leftVec[0], frontPos[1] + leftVec[1]}
		pathToFollow = append(pathToFollow, frontLeftPost)
	case 3:
		leftPos := RelativeCoordinates{leftVec[0] + pos[0], leftVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, leftPos)
	case 4:
		leftPos := RelativeCoordinates{leftVec[0] + pos[0], leftVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, leftPos)
		rearLeftPos := RelativeCoordinates{leftPos[0] + rearVec[0], leftPos[1] + rearVec[1]}
		pathToFollow = append(pathToFollow, rearLeftPos)
	case 5:
		rearPos := RelativeCoordinates{rearVec[0] + pos[0], rearVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, rearPos)
	case 6:
		rightPos := RelativeCoordinates{rightVec[0] + pos[0], rightVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, rightPos)
		rightRearPos := RelativeCoordinates{rightPos[0] + rearVec[0], rightPos[1] + rearVec[1]}
		pathToFollow = append(pathToFollow, rightRearPos)
	case 7:
		rightPos := RelativeCoordinates{rightVec[0] + pos[0], rightVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, rightPos)
	case 8:
		frontPos := RelativeCoordinates{frontVec[0] + pos[0], frontVec[1] + pos[1]}
		pathToFollow = append(pathToFollow, frontPos)
		frontRight := RelativeCoordinates{frontPos[0] + rightVec[0], frontPos[1] + rightVec[1]}
		pathToFollow = append(pathToFollow, frontRight)
	default:
		log.Println("Invalid message direction:", direction)
	}
	for _, point := range pathToFollow {
		game.moveToTile(point)
		game.Socket.SendCommand(network.LookAround, network.EmptyBody)
		_ = game.awaitResponseToCommand()
		game.updateFrequency()
		game.updatePrioritiesFromViewMap()
	}
}
