package ai

import (
	"reflect"
	"testing"
	"zappy_ai/network"
)

func Test_updatePosition(t *testing.T) {
	type args struct {
		position  RelativeCoordinates
		worldSize RelativeCoordinates
		direction network.PlayerDirection
	}
	tests := []struct {
		name string
		args args
		want RelativeCoordinates
	}{
		{"Up", args{RelativeCoordinates{0, 0}, RelativeCoordinates{10, 10}, network.Up}, RelativeCoordinates{0, 1}},
		{"Up wrapping", args{RelativeCoordinates{0, 9}, RelativeCoordinates{10, 10}, network.Up}, RelativeCoordinates{0, 0}},
		{"Down", args{RelativeCoordinates{0, 9}, RelativeCoordinates{10, 10}, network.Down}, RelativeCoordinates{0, 8}},
		{"Down wrapping", args{RelativeCoordinates{0, 0}, RelativeCoordinates{10, 10}, network.Down}, RelativeCoordinates{0, 9}},
		{"Left", args{RelativeCoordinates{5, 0}, RelativeCoordinates{10, 10}, network.Left}, RelativeCoordinates{4, 0}},
		{"Left wrapping", args{RelativeCoordinates{0, 0}, RelativeCoordinates{10, 10}, network.Left}, RelativeCoordinates{9, 0}},
		{"Right", args{RelativeCoordinates{0, 0}, RelativeCoordinates{10, 10}, network.Right}, RelativeCoordinates{1, 0}},
		{"Right wrapping", args{RelativeCoordinates{9, 0}, RelativeCoordinates{10, 10}, network.Right}, RelativeCoordinates{0, 0}},
		{"Invalid direction", args{RelativeCoordinates{5, 5}, RelativeCoordinates{10, 10}, network.PlayerDirection(5)}, RelativeCoordinates{5, 5}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := updatePosition(tt.args.position, tt.args.worldSize, tt.args.direction); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("updatePosition() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_getMiddlePoints(t *testing.T) {
	type args struct {
		origin      RelativeCoordinates
		destination RelativeCoordinates
		worldSize   RelativeCoordinates
		pqueue      PriorityQueue
	}

	argsValid := args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{6, 6}, worldSize: RelativeCoordinates{20, 20}, pqueue: PriorityQueue{
		&Item{value: RelativeCoordinates{1, 4}, priority: 2, originalPriority: 3, index: 0},
		&Item{value: RelativeCoordinates{5, 15}, priority: 1, originalPriority: 5, index: 1},
		&Item{value: RelativeCoordinates{6, 2}, priority: 0, originalPriority: 4, index: 2},
	}}
	argsValidWrap := args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{5, 5}, worldSize: RelativeCoordinates{6, 6},
		pqueue: PriorityQueue{
			&Item{value: RelativeCoordinates{3, 2}, originalPriority: 5},
			&Item{value: RelativeCoordinates{6, 5}, originalPriority: 4},
			&Item{value: RelativeCoordinates{1, 2}, originalPriority: 3},
			&Item{value: RelativeCoordinates{0, 5}, originalPriority: 3},
			&Item{value: RelativeCoordinates{5, 6}, originalPriority: 2},
		}}

	tests := []struct {
		name string
		args args
		want []*Item
	}{
		{"No points", args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{6, 6}, worldSize: RelativeCoordinates{6, 6}, pqueue: PriorityQueue{}}, []*Item{}},
		{"No valid points",
			args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{6, 6}, worldSize: RelativeCoordinates{10, 10}, pqueue: PriorityQueue{
				&Item{value: RelativeCoordinates{0, 5}, priority: 2, originalPriority: 3, index: 0},
				&Item{value: RelativeCoordinates{5, 15}, priority: 1, originalPriority: 5, index: 1},
				&Item{value: RelativeCoordinates{8, 2}, priority: 0, originalPriority: 4, index: 2},
			}}, []*Item{},
		},
		{"Valid points",
			argsValid, []*Item{argsValid.pqueue[0], argsValid.pqueue[2]},
		},
		{"Valid points wrapping", argsValidWrap, []*Item{argsValidWrap.pqueue[3]}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := getMiddlePoints(tt.args.origin, tt.args.destination, tt.args.worldSize, tt.args.pqueue); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("getMiddlePoints() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_computeOptimalPath(t *testing.T) {
	type args struct {
		origin       RelativeCoordinates
		destination  RelativeCoordinates
		worldSize    RelativeCoordinates
		middlePoints []*Item
	}
	tests := []struct {
		name string
		args args
		want []RelativeCoordinates
	}{
		{"Basic test 1", args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{5, 5}, worldSize: RelativeCoordinates{10, 10},
			middlePoints: []*Item{&Item{value: RelativeCoordinates{1, 2}, originalPriority: 3},
				&Item{value: RelativeCoordinates{3, 2}, originalPriority: 5},
				&Item{value: RelativeCoordinates{3, 4}, originalPriority: 4},
				&Item{value: RelativeCoordinates{5, 4}, originalPriority: 2},
				&Item{value: RelativeCoordinates{4, 3}, originalPriority: 3}}},
			[]RelativeCoordinates{{1, 2}, {3, 2}, {3, 4}, {5, 4}}},
		{"Basic test 2", args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{5, 5}, worldSize: RelativeCoordinates{10, 10},
			middlePoints: []*Item{&Item{value: RelativeCoordinates{1, 2}, originalPriority: 3},
				&Item{value: RelativeCoordinates{3, 2}, originalPriority: 5},
				&Item{value: RelativeCoordinates{5, 4}, originalPriority: 2},
				&Item{value: RelativeCoordinates{4, 3}, originalPriority: 3}}},
			[]RelativeCoordinates{{1, 2}, {3, 2}, {4, 3}, {5, 4}}},
		{"Basic test 3", args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{5, 5}, worldSize: RelativeCoordinates{6, 6},
			middlePoints: []*Item{&Item{value: RelativeCoordinates{2, 2}, originalPriority: 1}}},
			[]RelativeCoordinates{{2, 2}}},
		{"Basic test wrapping X", args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{5, 5}, worldSize: RelativeCoordinates{6, 15},
			middlePoints: []*Item{&Item{value: RelativeCoordinates{3, 2}, originalPriority: 5},
				&Item{value: RelativeCoordinates{6, 5}, originalPriority: 4},
				&Item{value: RelativeCoordinates{1, 2}, originalPriority: 3},
				&Item{value: RelativeCoordinates{0, 2}, originalPriority: 3},
				&Item{value: RelativeCoordinates{5, 2}, originalPriority: 2}}},
			[]RelativeCoordinates{{0, 2}, {6, 5}}},
		{"Basic test wrapping both ways", args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{5, 5}, worldSize: RelativeCoordinates{6, 6},
			middlePoints: []*Item{&Item{value: RelativeCoordinates{3, 2}, originalPriority: 5},
				&Item{value: RelativeCoordinates{6, 5}, originalPriority: 4},
				&Item{value: RelativeCoordinates{1, 2}, originalPriority: 3},
				&Item{value: RelativeCoordinates{0, 2}, originalPriority: 3},
				&Item{value: RelativeCoordinates{5, 6}, originalPriority: 2}}},
			[]RelativeCoordinates{{6, 5}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := computeOptimalPath(tt.args.origin, tt.args.destination, tt.args.worldSize, tt.args.middlePoints); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("computeOptimalPath() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_computeBasicPath(t *testing.T) {
	type args struct {
		origin      RelativeCoordinates
		destination RelativeCoordinates
		worldSize   RelativeCoordinates
		xDistance   int
		yDistance   int
		xSign       int
		ySign       int
	}
	tests := []struct {
		name string
		args args
		want []RelativeCoordinates
	}{
		{"Basic test", args{origin: RelativeCoordinates{0, 0}, destination: RelativeCoordinates{2, 3}, worldSize: RelativeCoordinates{5, 5}, xDistance: 2, yDistance: 3, xSign: 1, ySign: 1},
			[]RelativeCoordinates{{0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 3}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := computeBasicPath(tt.args.origin, tt.args.destination, tt.args.worldSize, tt.args.xDistance, tt.args.yDistance, tt.args.xSign, tt.args.ySign); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("computeBasicPath() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_getTileDirection(t *testing.T) {
	type args struct {
		pos       RelativeCoordinates
		tile      RelativeCoordinates
		worldSize RelativeCoordinates
	}
	tests := []struct {
		name string
		args args
		want network.PlayerDirection
	}{
		{"Direction Right", args{pos: RelativeCoordinates{1, 1}, tile: RelativeCoordinates{2, 1}, worldSize: RelativeCoordinates{10, 10}}, network.Right},
		{"Direction Right Wrap", args{pos: RelativeCoordinates{9, 1}, tile: RelativeCoordinates{0, 1}, worldSize: RelativeCoordinates{10, 10}}, network.Right},
		{"Direction Left", args{pos: RelativeCoordinates{1, 1}, tile: RelativeCoordinates{0, 1}, worldSize: RelativeCoordinates{10, 10}}, network.Left},
		{"Direction Left Wrap", args{pos: RelativeCoordinates{0, 1}, tile: RelativeCoordinates{9, 1}, worldSize: RelativeCoordinates{10, 10}}, network.Left},
		{"Direction Down", args{pos: RelativeCoordinates{1, 1}, tile: RelativeCoordinates{1, 0}, worldSize: RelativeCoordinates{10, 10}}, network.Down},
		{"Direction Down Wrap", args{pos: RelativeCoordinates{1, 0}, tile: RelativeCoordinates{1, 9}, worldSize: RelativeCoordinates{10, 10}}, network.Down},
		{"Direction Up", args{pos: RelativeCoordinates{1, 1}, tile: RelativeCoordinates{1, 2}, worldSize: RelativeCoordinates{10, 10}}, network.Up},
		{"Direction Up Wrap", args{pos: RelativeCoordinates{1, 9}, tile: RelativeCoordinates{1, 0}, worldSize: RelativeCoordinates{10, 10}}, network.Up},
		{"Invalid direction same tile", args{pos: RelativeCoordinates{1, 1}, tile: RelativeCoordinates{1, 1}, worldSize: RelativeCoordinates{10, 10}}, -1},
		{"Invalid direction diagonal", args{pos: RelativeCoordinates{1, 1}, tile: RelativeCoordinates{2, 2}, worldSize: RelativeCoordinates{10, 10}}, -1},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := getTileDirection(tt.args.pos, tt.args.tile, tt.args.worldSize); got != tt.want {
				t.Errorf("getTileDirection() = %v, want %v", got, tt.want)
			}
		})
	}
}
