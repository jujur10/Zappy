package ai

import "container/heap"

// Priority queue implementation is from Go stdlib documentation

// An Item is something we manage in a priority queue.
type Item struct {
	value            RelativeCoordinates // The value of the item; arbitrary.
	priority         int                 // The priority of the item in the queue. Diminishes with distance
	originalPriority int                 // The originalPriority of the tile
	// The index is needed by Update and is maintained by the heap.Interface methods.
	index int // The index of the item in the heap.
}

// A PriorityQueue implements heap.Interface and holds Items.
type PriorityQueue []*Item

// Len returns the length of the priority queue
func (pq PriorityQueue) Len() int { return len(pq) }

// Less checks if the priority of the element at index j is lower than the one at index i
func (pq PriorityQueue) Less(i, j int) bool {
	// We want Pop to give us the highest, not lowest, priority so we use greater than here.
	return pq[i].priority > pq[j].priority
}

// Swap two element in the priority queue
// Interface for heap package, should NOT be used
func (pq PriorityQueue) Swap(i, j int) {
	pq[i], pq[j] = pq[j], pq[i]
	pq[i].index = i
	pq[j].index = j
}

// Push adds an item to the priority queue
// Interface for heap package, should NOT be used
func (pq *PriorityQueue) Push(x any) {
	n := len(*pq)
	item := x.(*Item)
	item.index = n
	*pq = append(*pq, item)
}

// Pop removes an item from the priority queue and returns it
// Interface for heap package, should NOT be used
func (pq *PriorityQueue) Pop() any {
	old := *pq
	n := len(old)
	item := old[n-1]
	old[n-1] = nil  // avoid memory leak
	item.index = -1 // for safety
	*pq = old[0 : n-1]
	return item
}

// Update modifies the priority and value of an Item in the queue.
func (pq *PriorityQueue) Update(item *Item, value RelativeCoordinates, priority int) {
	item.value = value
	item.priority = priority
	heap.Fix(pq, item.index)
}
