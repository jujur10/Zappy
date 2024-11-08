package ai

import (
	"testing"
	"time"
	"zappy_ai/network"
)

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
		pos1      RelativeCoordinates
		pos2      RelativeCoordinates
		worldSize RelativeCoordinates
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{"Distance 0", args{RelativeCoordinates{0, 0}, RelativeCoordinates{0, 0}, RelativeCoordinates{10, 10}}, 0},
		{"Distance 0 bis", args{RelativeCoordinates{20, 60}, RelativeCoordinates{20, 60}, RelativeCoordinates{75, 75}}, 0},
		{"Basic test 1", args{RelativeCoordinates{0, 0}, RelativeCoordinates{3, 1}, RelativeCoordinates{10, 10}}, 4},
		{"Basic test 2", args{RelativeCoordinates{12, 5}, RelativeCoordinates{20, 6}, RelativeCoordinates{30, 30}}, 9},
		{"Basic test 3", args{RelativeCoordinates{2, 14}, RelativeCoordinates{6, 2}, RelativeCoordinates{30, 30}}, 16},
		{"Basic test 4", args{RelativeCoordinates{256, 128}, RelativeCoordinates{128, 256}, RelativeCoordinates{512, 512}}, 256},
		{"Test wrapping 1", args{RelativeCoordinates{0, 2}, RelativeCoordinates{9, 0}, RelativeCoordinates{10, 10}}, 3},
		{"Test wrapping 2", args{RelativeCoordinates{3, 2}, RelativeCoordinates{2, 9}, RelativeCoordinates{10, 10}}, 4},
		{"Test wrapping 3", args{RelativeCoordinates{80, 90}, RelativeCoordinates{10, 10}, RelativeCoordinates{100, 100}}, 50},
		{"Test wrapping 4", args{RelativeCoordinates{10, 10}, RelativeCoordinates{80, 90}, RelativeCoordinates{100, 100}}, 50},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := ManhattanDistance(tt.args.pos1, tt.args.pos2, tt.args.worldSize); got != tt.want {
				t.Errorf("ManhattanDistance() = %v, want %v", got, tt.want)
			}
		})
	}
}

// Default game struct
// fields{View: make(ViewMap, 0), Inventory: make(Inventory),
//	TimeStep: time.Duration(1), TeamName: "",
//	Socket: network.ServerConn{Reader: nil, LastCommandType: network.None, Connection: nil},
//	Coordinates: WorldCoords{CoordsFromOrigin: RelativeCoordinates{0,0}},
//	TilesQueue: PriorityQueue{}, Level: 1, TotalResourcesRequired: totalResourcesRequired,
//	LevelUpResources: levelUpResources, FoodManager: FoodManagement{FoodPriority: 4, FoodChannel: make(chan int)}}

func TestGame_getLevelUpPriority(t *testing.T) {
	type fields struct {
		View                   ViewMap
		Inventory              Inventory
		TimeStep               time.Duration
		TeamName               string
		Socket                 network.ServerConn
		Coordinates            WorldCoords
		Movement               MovementData
		Level                  int
		LevelUpResources       map[int]Inventory
		TotalResourcesRequired Inventory
		FoodManager            FoodManagement
		MessageManager         MessagesManagement
	}
	tests := []struct {
		name   string
		fields fields
		want   int
	}{
		{"Test not enough food", fields{Level: 1, TotalResourcesRequired: totalResourcesRequired,
			LevelUpResources: levelUpResources, FoodManager: FoodManagement{FoodPriority: 9, InputFoodChannel: make(chan int)}},
			0},

		{"Test not enough resources", fields{Level: 1, TotalResourcesRequired: totalResourcesRequired,
			LevelUpResources: levelUpResources, FoodManager: FoodManagement{FoodPriority: 4, InputFoodChannel: make(chan int)}},
			0},

		{"Valid test level 1", fields{Level: 1, TotalResourcesRequired: totalResourcesRequired,
			LevelUpResources: map[int]Inventory{1: {Player: 1, Linemate: 0, Deraumere: 0, Sibur: 0, Mendiane: 0, Phiras: 0, Thystame: 0}},
			FoodManager:      FoodManagement{FoodPriority: 4, InputFoodChannel: make(chan int)}},
			levelUpPriority},

		{"Valid test level 5", fields{Level: 1, TotalResourcesRequired: totalResourcesRequired,
			LevelUpResources: map[int]Inventory{5: {Player: 4, Linemate: 0, Deraumere: 0, Sibur: 0, Mendiane: 0, Phiras: 0, Thystame: 0}},
			FoodManager:      FoodManagement{FoodPriority: 4, InputFoodChannel: make(chan int)}},
			levelUpPriority},
		{"Level up leech available", fields{Level: 2, FoodManager: FoodManagement{FoodPriority: 5},
			MessageManager: MessagesManagement{waitingForLevelUp: false, UUID: "azerty1234",
				messageStatusList: []broadcastMessageContent{{uuid: "aaa123", targetLevel: 3}}}}, leechLevelUpPriority},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := Game{
				View:                   tt.fields.View,
				Inventory:              tt.fields.Inventory,
				TimeStep:               tt.fields.TimeStep,
				TeamName:               tt.fields.TeamName,
				Socket:                 tt.fields.Socket,
				Coordinates:            tt.fields.Coordinates,
				Movement:               tt.fields.Movement,
				Level:                  tt.fields.Level,
				LevelUpResources:       tt.fields.LevelUpResources,
				TotalResourcesRequired: tt.fields.TotalResourcesRequired,
				FoodManager:            tt.fields.FoodManager,
				MessageManager:         tt.fields.MessageManager,
			}
			if got := game.getLevelUpPriority(); got != tt.want {
				t.Errorf("getLevelUpPriority() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGame_getResourcePriority(t *testing.T) {
	type fields struct {
		View                   ViewMap
		Inventory              Inventory
		TimeStep               time.Duration
		TeamName               string
		Socket                 network.ServerConn
		Coordinates            WorldCoords
		Movement               MovementData
		Level                  int
		LevelUpResources       map[int]Inventory
		TotalResourcesRequired Inventory
		FoodManager            FoodManagement
	}
	type args struct {
		item TileItem
	}
	tests := []struct {
		name   string
		fields fields
		args   args
		want   int
	}{
		{"Resource is food", fields{FoodManager: FoodManagement{FoodPriority: 3, InputFoodChannel: make(chan int)}}, args{Food}, 3},
		{"Resource is player", fields{}, args{Player}, 0},
		{"Resource not needed", fields{Level: 1, LevelUpResources: map[int]Inventory{
			1: {}, 2: {}, 3: {}, 4: {}, 5: {}, 6: {}, 7: {}}}, args{Thystame}, 0},
		{"Resource needed at level 3", fields{Level: 1, LevelUpResources: levelUpResources}, args{Phiras}, 6},
		{"Resource needed at current level", fields{Level: 2, LevelUpResources: levelUpResources}, args{Deraumere}, 8},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := Game{
				View:                   tt.fields.View,
				Inventory:              tt.fields.Inventory,
				TimeStep:               tt.fields.TimeStep,
				TeamName:               tt.fields.TeamName,
				Socket:                 tt.fields.Socket,
				Coordinates:            tt.fields.Coordinates,
				Movement:               tt.fields.Movement,
				Level:                  tt.fields.Level,
				LevelUpResources:       tt.fields.LevelUpResources,
				TotalResourcesRequired: tt.fields.TotalResourcesRequired,
				FoodManager:            tt.fields.FoodManager,
			}
			if got := game.getResourcePriority(tt.args.item); got != tt.want {
				t.Errorf("getResourcePriority() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGame_getTilePriority(t *testing.T) {
	type fields struct {
		View                   ViewMap
		Inventory              Inventory
		TimeStep               time.Duration
		TeamName               string
		Socket                 network.ServerConn
		Coordinates            WorldCoords
		Movement               MovementData
		Level                  int
		LevelUpResources       map[int]Inventory
		TotalResourcesRequired Inventory
		FoodManager            FoodManagement
	}
	type args struct {
		tile []TileItem
	}
	tests := []struct {
		name   string
		fields fields
		args   args
		want   int
	}{
		{"Tile with player and food", fields{FoodManager: FoodManagement{FoodPriority: 5, InputFoodChannel: make(chan int)}}, args{[]TileItem{Player, Food}}, 0},
		{"Tile with only player", fields{}, args{[]TileItem{Player}}, 0},
		{"Tile with unneeded resource and player", fields{Level: 1, LevelUpResources: map[int]Inventory{
			1: {}, 2: {}, 3: {}, 4: {}, 5: {}, 6: {}, 7: {}}}, args{[]TileItem{Player, Thystame}}, 0},
		{"Tile with unneeded resource and food", fields{FoodManager: FoodManagement{FoodPriority: 4, InputFoodChannel: make(chan int)},
			Level: 1, LevelUpResources: map[int]Inventory{1: {}, 2: {}, 3: {}, 4: {}, 5: {}, 6: {}, 7: {}}},
			args{[]TileItem{Food, Thystame}}, 4},
		{"Tile with resource needed at level 3", fields{Level: 1, LevelUpResources: levelUpResources}, args{[]TileItem{Phiras}}, 6},
		{"Tile with resource needed at level 8 and at level 2", fields{Level: 1, LevelUpResources: levelUpResources}, args{[]TileItem{Deraumere, Thystame}}, 7},
		{"Resource needed at current level and at level 5", fields{Level: 2, LevelUpResources: levelUpResources}, args{[]TileItem{Mendiane, Sibur}}, 8},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := Game{
				View:                   tt.fields.View,
				Inventory:              tt.fields.Inventory,
				TimeStep:               tt.fields.TimeStep,
				TeamName:               tt.fields.TeamName,
				Socket:                 tt.fields.Socket,
				Coordinates:            tt.fields.Coordinates,
				Movement:               tt.fields.Movement,
				Level:                  tt.fields.Level,
				LevelUpResources:       tt.fields.LevelUpResources,
				TotalResourcesRequired: tt.fields.TotalResourcesRequired,
				FoodManager:            tt.fields.FoodManager,
			}
			if got := game.getTilePriority(tt.args.tile); got != tt.want {
				t.Errorf("getTilePriority() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGame_isResourceRequired(t *testing.T) {
	type fields struct {
		View                   ViewMap
		Inventory              Inventory
		TimeStep               time.Duration
		TeamName               string
		Socket                 network.ServerConn
		Coordinates            WorldCoords
		Movement               MovementData
		Level                  int
		LevelUpResources       map[int]Inventory
		TotalResourcesRequired Inventory
		FoodManager            FoodManagement
	}
	type args struct {
		item TileItem
	}
	tests := []struct {
		name   string
		fields fields
		args   args
		want   bool
	}{
		{"Resource is food", fields{}, args{Food}, true},
		{"Resource is player", fields{}, args{Player}, false},
		{"Resource not needed", fields{TotalResourcesRequired: make(Inventory)}, args{Thystame}, false},
		{"Resource needed 1", fields{TotalResourcesRequired: totalResourcesRequired}, args{Phiras}, true},
		{"Resource needed 2", fields{TotalResourcesRequired: totalResourcesRequired}, args{Deraumere}, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := Game{
				View:                   tt.fields.View,
				Inventory:              tt.fields.Inventory,
				TimeStep:               tt.fields.TimeStep,
				TeamName:               tt.fields.TeamName,
				Socket:                 tt.fields.Socket,
				Coordinates:            tt.fields.Coordinates,
				Movement:               tt.fields.Movement,
				Level:                  tt.fields.Level,
				LevelUpResources:       tt.fields.LevelUpResources,
				TotalResourcesRequired: tt.fields.TotalResourcesRequired,
				FoodManager:            tt.fields.FoodManager,
			}
			if got := game.isResourceRequired(tt.args.item); got != tt.want {
				t.Errorf("isResourceRequired() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGame_removeLevelUpResources(t *testing.T) {
	type fields struct {
		View                   ViewMap
		Inventory              Inventory
		TimeStep               time.Duration
		TeamName               string
		Socket                 network.ServerConn
		Coordinates            WorldCoords
		Movement               MovementData
		Level                  int
		LevelUpResources       map[int]Inventory
		TotalResourcesRequired Inventory
		FoodManager            FoodManagement
	}
	tests := []struct {
		name   string
		fields fields
	}{
		{"Remove level 1 resources", fields{Level: 1, LevelUpResources: levelUpResources, TotalResourcesRequired: totalResourcesRequired}},
		{"Remove level 4 resources", fields{Level: 4, LevelUpResources: levelUpResources, TotalResourcesRequired: totalResourcesRequired}},
		{"Remove level 7 resources", fields{Level: 7, LevelUpResources: levelUpResources, TotalResourcesRequired: totalResourcesRequired}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := Game{
				View:                   tt.fields.View,
				Inventory:              tt.fields.Inventory,
				TimeStep:               tt.fields.TimeStep,
				TeamName:               tt.fields.TeamName,
				Socket:                 tt.fields.Socket,
				Coordinates:            tt.fields.Coordinates,
				Movement:               tt.fields.Movement,
				Level:                  tt.fields.Level,
				LevelUpResources:       tt.fields.LevelUpResources,
				TotalResourcesRequired: tt.fields.TotalResourcesRequired,
				FoodManager:            tt.fields.FoodManager,
			}
			game.removeLevelUpResources()
		})
	}
}

func TGameResourceCollectedHelper(channel chan int) {
	select {
	case _, _ = <-channel:
		return
	}
}

func TestGame_resourceCollected(t *testing.T) {
	type fields struct {
		View                   ViewMap
		Inventory              Inventory
		TimeStep               time.Duration
		TeamName               string
		Socket                 network.ServerConn
		Coordinates            WorldCoords
		Movement               MovementData
		Level                  int
		LevelUpResources       map[int]Inventory
		TotalResourcesRequired Inventory
		FoodManager            FoodManagement
	}
	type args struct {
		item TileItem
	}
	foodChannel := make(chan int)
	go TGameResourceCollectedHelper(foodChannel)
	tests := []struct {
		name   string
		fields fields
		args   args
	}{
		{"Resource collected is player", fields{}, args{Player}},
		{"Resource collected is food", fields{FoodManager: FoodManagement{FoodPriority: 0, InputFoodChannel: foodChannel}}, args{Food}},
		{"Resource collected is required stone", fields{Level: 1, LevelUpResources: levelUpResources, TotalResourcesRequired: totalResourcesRequired}, args{Deraumere}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := Game{
				View:                   tt.fields.View,
				Inventory:              tt.fields.Inventory,
				TimeStep:               tt.fields.TimeStep,
				TeamName:               tt.fields.TeamName,
				Socket:                 tt.fields.Socket,
				Coordinates:            tt.fields.Coordinates,
				Movement:               tt.fields.Movement,
				Level:                  tt.fields.Level,
				LevelUpResources:       tt.fields.LevelUpResources,
				TotalResourcesRequired: tt.fields.TotalResourcesRequired,
				FoodManager:            tt.fields.FoodManager,
			}
			game.resourceCollected(tt.args.item)
		})
	}
}

func TestGame_getCurrentTilePriority(t *testing.T) {
	type fields struct {
		View                   ViewMap
		Inventory              Inventory
		TimeStep               time.Duration
		TeamName               string
		Socket                 network.ServerConn
		Coordinates            WorldCoords
		Movement               MovementData
		Level                  int
		LevelUpResources       map[int]Inventory
		TotalResourcesRequired Inventory
		FoodManager            FoodManagement
		MessageManager         MessagesManagement
		SlotsLeft              int
	}
	type args struct {
		tile []TileItem
	}
	tests := []struct {
		name   string
		fields fields
		args   args
		want   int
	}{
		{"Tile with player and food", fields{FoodManager: FoodManagement{FoodPriority: 5, InputFoodChannel: make(chan int)}}, args{[]TileItem{Player, Food}}, 5},
		{"Tile with two players and food", fields{FoodManager: FoodManagement{FoodPriority: 5, InputFoodChannel: make(chan int)}}, args{[]TileItem{Player, Food, Player}}, 0},
		{"Tile with only player", fields{}, args{[]TileItem{Player}}, 0},
		{"Tile with unneeded resource and player", fields{Level: 1, LevelUpResources: map[int]Inventory{
			1: {}, 2: {}, 3: {}, 4: {}, 5: {}, 6: {}, 7: {}}}, args{[]TileItem{Player, Thystame}}, 0},
		{"Tile with unneeded resource and food", fields{FoodManager: FoodManagement{FoodPriority: 4, InputFoodChannel: make(chan int)},
			Level: 1, LevelUpResources: map[int]Inventory{1: {}, 2: {}, 3: {}, 4: {}, 5: {}, 6: {}, 7: {}}},
			args{[]TileItem{Food, Thystame}}, 4},
		{"Tile with resource needed at level 3", fields{Level: 1, LevelUpResources: resourcesToDrop}, args{[]TileItem{Phiras}}, 6},
		{"Tile with resource needed at level 8 and at level 2", fields{Level: 1, LevelUpResources: resourcesToDrop}, args{[]TileItem{Deraumere, Thystame}}, 7},
		{"Resource needed at current level and at level 5", fields{Level: 2, LevelUpResources: resourcesToDrop}, args{[]TileItem{Mendiane, Sibur}}, 8},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := &Game{
				View:                   tt.fields.View,
				Inventory:              tt.fields.Inventory,
				TimeStep:               tt.fields.TimeStep,
				TeamName:               tt.fields.TeamName,
				Socket:                 tt.fields.Socket,
				Coordinates:            tt.fields.Coordinates,
				Movement:               tt.fields.Movement,
				Level:                  tt.fields.Level,
				LevelUpResources:       tt.fields.LevelUpResources,
				TotalResourcesRequired: tt.fields.TotalResourcesRequired,
				FoodManager:            tt.fields.FoodManager,
				MessageManager:         tt.fields.MessageManager,
				SlotsLeft:              tt.fields.SlotsLeft,
			}
			if got := game.getCurrentTilePriority(tt.args.tile); got != tt.want {
				t.Errorf("getCurrentTilePriority() = %v, want %v", got, tt.want)
			}
		})
	}
}
