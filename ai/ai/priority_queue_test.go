package ai

import (
	"container/heap"
	"testing"
)

func TestPriorityQueue_Len(t *testing.T) {
	items := map[RelativeCoordinates]int{
		RelativeCoordinates{-1, 2}: 3, RelativeCoordinates{5, 1}: 2, RelativeCoordinates{52, 44}: 4, RelativeCoordinates{1, 2}: 3,
	}
	pq := make(PriorityQueue, len(items))
	i := 0
	for value, priority := range items {
		pq[i] = &Item{
			value:    value,
			priority: priority,
			index:    i,
		}
		i++
	}
	heap.Init(&pq)

	t.Run("Len 4 elements", func(t *testing.T) {
		if got := pq.Len(); got != 4 {
			t.Errorf("Len() = %v, want %v", got, 4)
		}
	})

	item := &Item{
		value:    RelativeCoordinates{3, 0},
		priority: 1,
	}
	heap.Push(&pq, item)
	pq.Update(item, item.value, 5)

	t.Run("Len 5 elements push", func(t *testing.T) {
		if got := pq.Len(); got != 5 {
			t.Errorf("Len() = %v, want %v", got, 5)
		}
	})

	_ = heap.Pop(&pq)
	_ = heap.Pop(&pq)

	t.Run("Len 3 elements 2x pop", func(t *testing.T) {
		if got := pq.Len(); got != 3 {
			t.Errorf("Len() = %v, want %v", got, 3)
		}
	})
}

func TestPriorityQueue_Less(t *testing.T) {
	items := map[RelativeCoordinates]int{
		RelativeCoordinates{-1, 2}: 3, RelativeCoordinates{5, 1}: 2, RelativeCoordinates{52, 44}: 4, RelativeCoordinates{1, 2}: 3,
	}
	pq := make(PriorityQueue, len(items))
	i := 0
	for value, priority := range items {
		pq[i] = &Item{
			value:    value,
			priority: priority,
			index:    i,
		}
		i++
	}
	heap.Init(&pq)

	t.Run("Is elem 0 less than elem 3", func(t *testing.T) {
		if got := pq.Less(3, 0); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})

	t.Run("Is elem 1 less than elem 2", func(t *testing.T) {
		if got := pq.Less(2, 1); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})

	item := &Item{
		value:    RelativeCoordinates{3, 0},
		priority: 1,
	}
	heap.Push(&pq, item)
	pq.Update(item, item.value, 5)

	t.Run("Is elem 0 less than elem 4", func(t *testing.T) {
		if got := pq.Less(4, 0); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})

	_ = heap.Pop(&pq)
	_ = heap.Pop(&pq)
	t.Run("Is elem 0 less than elem 2", func(t *testing.T) {
		if got := pq.Less(2, 0); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})
}

func TestPriorityQueue_Update(t *testing.T) {
	items := map[RelativeCoordinates]int{
		RelativeCoordinates{-1, 2}: 3, RelativeCoordinates{5, 1}: 2, RelativeCoordinates{52, 44}: 4, RelativeCoordinates{1, 2}: 3,
	}
	pq := make(PriorityQueue, len(items))
	i := 0
	for value, priority := range items {
		pq[i] = &Item{
			value:    value,
			priority: priority,
			index:    i,
		}
		i++
	}
	heap.Init(&pq)
	item := &Item{
		value:    RelativeCoordinates{3, 0},
		priority: 1,
	}
	heap.Push(&pq, item)
	t.Run("Change value of newly added item", func(t *testing.T) {
		pq.Update(item, item.value, 5)
	})

	type args struct {
		item     *Item
		value    RelativeCoordinates
		priority int
	}
	tests := []struct {
		name string
		pq   PriorityQueue
		args args
	}{
		{"Change value of random item 1", pq, args{item: pq[2], value: pq[2].value, priority: 7}},
		{"Change value of random item 2", pq, args{item: pq[3], value: pq[3].value, priority: 1}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			tt.pq.Update(tt.args.item, tt.args.value, tt.args.priority)
		})
	}
	_ = heap.Pop(&pq)
	_ = heap.Pop(&pq)

	t.Run("Change value of random item after pop", func(t *testing.T) {
		pq.Update(pq[1], pq[1].value, 2)
	})
}

func Test_graphPriorityQueue_Len(t *testing.T) {
	items := []graphNode{
		{pos: RelativeCoordinates{-1, 2}}, {pos: RelativeCoordinates{5, 1}}, {pos: RelativeCoordinates{52, 44}}, {pos: RelativeCoordinates{1, 2}},
	}
	prios := []int{3, 2, 4, 3}
	pq := make(graphPriorityQueue, len(items))
	i := 0
	for idx, node := range items {
		pq[i] = &graphItem{
			node:     &node,
			priority: prios[idx],
			index:    i,
		}
		i++
	}
	heap.Init(&pq)

	t.Run("Len 4 elements", func(t *testing.T) {
		if got := pq.Len(); got != 4 {
			t.Errorf("Len() = %v, want %v", got, 4)
		}
	})

	item := &graphItem{
		node:     &graphNode{pos: RelativeCoordinates{3, 0}},
		priority: 1,
	}
	heap.Push(&pq, item)
	pq.Update(item, item.node, 5)

	t.Run("Len 5 elements push", func(t *testing.T) {
		if got := pq.Len(); got != 5 {
			t.Errorf("Len() = %v, want %v", got, 5)
		}
	})

	_ = heap.Pop(&pq)
	_ = heap.Pop(&pq)

	t.Run("Len 3 elements 2x pop", func(t *testing.T) {
		if got := pq.Len(); got != 3 {
			t.Errorf("Len() = %v, want %v", got, 3)
		}
	})
}

func Test_graphPriorityQueue_Less(t *testing.T) {
	items := []graphNode{
		graphNode{pos: RelativeCoordinates{-1, 2}}, graphNode{pos: RelativeCoordinates{5, 1}}, graphNode{pos: RelativeCoordinates{52, 44}}, graphNode{pos: RelativeCoordinates{1, 2}},
	}
	prios := []int{3, 2, 4, 3}
	pq := make(graphPriorityQueue, len(items))
	i := 0
	for idx, node := range items {
		pq[i] = &graphItem{
			node:     &node,
			priority: prios[idx],
			index:    i,
		}
		i++
	}
	heap.Init(&pq)

	t.Run("Is elem 0 less than elem 3", func(t *testing.T) {
		if got := pq.Less(3, 0); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})

	t.Run("Is elem 1 less than elem 2", func(t *testing.T) {
		if got := pq.Less(2, 1); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})

	item := &graphItem{
		node:     &graphNode{pos: RelativeCoordinates{3, 0}},
		priority: 1,
	}
	heap.Push(&pq, item)
	pq.Update(item, item.node, 5)

	t.Run("Is elem 0 less than elem 4", func(t *testing.T) {
		if got := pq.Less(4, 0); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})

	_ = heap.Pop(&pq)
	_ = heap.Pop(&pq)
	t.Run("Is elem 0 less than elem 2", func(t *testing.T) {
		if got := pq.Less(2, 0); got != false {
			t.Errorf("Less() = %v, want %v", got, false)
		}
	})
}

func Test_graphPriorityQueue_Update(t *testing.T) {
	items := []graphNode{
		graphNode{pos: RelativeCoordinates{-1, 2}}, graphNode{pos: RelativeCoordinates{5, 1}}, graphNode{pos: RelativeCoordinates{52, 44}}, graphNode{pos: RelativeCoordinates{1, 2}},
	}
	prios := []int{3, 2, 4, 3}
	pq := make(graphPriorityQueue, len(items))
	i := 0
	for idx, node := range items {
		pq[i] = &graphItem{
			node:     &node,
			priority: prios[idx],
			index:    i,
		}
		i++
	}
	heap.Init(&pq)
	item := &graphItem{
		node:     &graphNode{pos: RelativeCoordinates{3, 0}},
		priority: 1,
	}
	heap.Push(&pq, item)
	t.Run("Change value of newly added item", func(t *testing.T) {
		pq.Update(item, item.node, 5)
	})

	type args struct {
		item     *graphItem
		node     *graphNode
		priority int
	}
	tests := []struct {
		name string
		pq   graphPriorityQueue
		args args
	}{
		{"Change value of random item 1", pq, args{item: pq[2], node: pq[2].node, priority: 7}},
		{"Change value of random item 2", pq, args{item: pq[3], node: pq[3].node, priority: 1}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			tt.pq.Update(tt.args.item, tt.args.node, tt.args.priority)
		})
	}
	_ = heap.Pop(&pq)
	_ = heap.Pop(&pq)

	t.Run("Change value of random item after pop", func(t *testing.T) {
		pq.Update(pq[1], pq[1].node, 2)
	})
}

func TestPriorityQueue_getPriorityQueueTileIndex(t *testing.T) {
	items := map[RelativeCoordinates]int{
		RelativeCoordinates{-1, 2}: 3, RelativeCoordinates{5, 1}: 2, RelativeCoordinates{52, 44}: 4, RelativeCoordinates{1, 2}: 3,
	}
	pq := make(PriorityQueue, len(items))
	i := 0
	for value, priority := range items {
		pq[i] = &Item{
			value:    value,
			priority: priority,
			index:    i,
		}
		i++
	}
	heap.Init(&pq)
	item := &Item{
		value:    RelativeCoordinates{3, 0},
		priority: 1,
	}
	heap.Push(&pq, item)
	pq.Update(item, item.value, 5)
	type args struct {
		tilePos RelativeCoordinates
	}
	tests := []struct {
		name string
		pq   PriorityQueue
		args args
		want int
	}{
		{"Basic test", pq, args{RelativeCoordinates{3, 0}}, 0},
		{"Test tile not in queue", pq, args{RelativeCoordinates{6, 4}}, -1},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := tt.pq.getPriorityQueueTileIndex(tt.args.tilePos); got != tt.want {
				t.Errorf("getPriorityQueueTileIndex() = %v, want %v", got, tt.want)
			}
		})
	}
}
