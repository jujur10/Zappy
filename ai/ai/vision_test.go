package ai

import (
	"reflect"
	"testing"
	"zappy_ai/network"
)

func TestCreateViewMap(t *testing.T) {
	type args struct {
		parsedList [][]string
	}
	tests := []struct {
		name    string
		args    args
		want    ViewMap
		wantErr bool
	}{
		{"Valid view map", args{[][]string{{"player"}, {"linemate", "food"}, {}, {"sibur"}, {"thystame"}, {}, {}, {"deraumere"}, {"mendiane"}}}, ViewMap{{Player}, {Linemate, Food}, {}, {Sibur}, {Thystame}, {}, {}, {Deraumere}, {Mendiane}}, false},
		{"Invalid argument", args{[][]string{{"player"}, {"linemate"}, {"phiras"}, {"sibure"}, {}, {}, {}, {"deraumere"}, {"mendiane"}}}, nil, true},
		{"Invalid number of tiles", args{[][]string{{"player"}, {"linemate"}, {}, {"sibur"}, {}, {}, {"deraumere"}, {"mendiane"}}}, nil, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := CreateViewMap(tt.args.parsedList)
			if (err != nil) != tt.wantErr {
				t.Errorf("CreateViewMap() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("CreateViewMap() got = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_buildAbsoluteCoordsMap(t *testing.T) {
	type args struct {
		direction  network.PlayerDirection
		worldSize  RelativeCoordinates
		position   RelativeCoordinates
		viewMapLen int
	}
	tests := []struct {
		name string
		args args
		want []RelativeCoordinates
	}{
		{"Level 1 ViewMap Up 10x10 at 0;0", args{direction: Up, worldSize: RelativeCoordinates{10, 10}, position: RelativeCoordinates{0, 0}, viewMapLen: 4},
			[]RelativeCoordinates{{0, 0}, {9, 1}, {0, 1}, {1, 1}}},
		{"Level 2 ViewMap Down 10x10 at 0;0", args{direction: Down, worldSize: RelativeCoordinates{10, 10}, position: RelativeCoordinates{0, 0}, viewMapLen: 9},
			[]RelativeCoordinates{{0, 0}, {1, 9}, {0, 9}, {9, 9}, {2, 8}, {1, 8}, {0, 8}, {9, 8}, {8, 8}}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := buildAbsoluteCoordsMap(tt.args.direction, tt.args.worldSize, tt.args.position, tt.args.viewMapLen); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("buildAbsoluteCoordsMap() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_getTileUsefulResources(t *testing.T) {
	type args struct {
		totalResourcesNeeded Inventory
		tile                 []TileItem
	}
	tests := []struct {
		name string
		args args
		want []TileItem
	}{
		{"Test no resources needed", args{totalResourcesNeeded: make(Inventory), tile: []TileItem{Linemate, Player, Phiras}}, []TileItem{}},
		{"Test only food", args{totalResourcesNeeded: totalResourcesRequired, tile: []TileItem{Food, Player, Food}}, []TileItem{Food, Food}},
		{"Normal test", args{totalResourcesNeeded: totalResourcesRequired, tile: []TileItem{Food, Player, Deraumere, Thystame, Mendiane}}, []TileItem{Food, Deraumere, Thystame, Mendiane}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := getTileUsefulResources(tt.args.totalResourcesNeeded, tt.args.tile); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("getTileUsefulResources() = %v, want %v", got, tt.want)
			}
		})
	}
}
