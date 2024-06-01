package ai

import "testing"

func TestAbs(t *testing.T) {
	type args struct {
		x int
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{"Basic test positive 1", args{3}, 3},
		{"Basic test positive 2", args{8489855}, 8489855},
		{"Basic test 0", args{0}, 0},
		{"Basic test negative 1", args{-1}, 1},
		{"Basic test negative 2", args{-552}, 552},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := Abs(tt.args.x); got != tt.want {
				t.Errorf("Abs() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestManhattanDistance(t *testing.T) {
	type args struct {
		pos1 RelativeCoordinates
		pos2 RelativeCoordinates
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{"Distance 0", args{RelativeCoordinates{0, 0}, RelativeCoordinates{0, 0}}, 0},
		{"Distance 0 bis", args{RelativeCoordinates{20, 60}, RelativeCoordinates{20, 60}}, 0},
		{"Basic test 1", args{RelativeCoordinates{0, 0}, RelativeCoordinates{3, 1}}, 4},
		{"Basic test 2", args{RelativeCoordinates{12, 5}, RelativeCoordinates{20, 6}}, 9},
		{"Basic test 3", args{RelativeCoordinates{2, -4}, RelativeCoordinates{-6, 2}}, 14},
		{"Basic test 4", args{RelativeCoordinates{256, 128}, RelativeCoordinates{128, 256}}, 256},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := ManhattanDistance(tt.args.pos1, tt.args.pos2); got != tt.want {
				t.Errorf("ManhattanDistance() = %v, want %v", got, tt.want)
			}
		})
	}
}
