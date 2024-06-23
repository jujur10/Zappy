package ai

import (
	"testing"
	"time"
	"zappy_ai/network"
)

func TestGame_isLevelUpLeechAvailable(t *testing.T) {
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
	tests := []struct {
		name   string
		fields fields
		want   bool
	}{
		{"Test waiting for lvl up", fields{MessageManager: MessagesManagement{waitingForLevelUp: true}}, false},
		{"Test no leech available", fields{Level: 4, MessageManager: MessagesManagement{UUID: "aaa123", waitingForLevelUp: false,
			messageStatusList: []broadcastMessageContent{
				{msgType: lvlUpComplete, uuid: "aab125", targetLevel: 4},
				{msgType: missingPlayers, uuid: "aas52", targetLevel: 6}}}}, false},
		{"Test leech available", fields{Level: 4, MessageManager: MessagesManagement{UUID: "aaa123", waitingForLevelUp: false,
			messageStatusList: []broadcastMessageContent{
				{msgType: lvlUpComplete, uuid: "aab125", targetLevel: 4},
				{msgType: missingPlayers, uuid: "aad566", targetLevel: 5},
				{msgType: missingPlayers, uuid: "aas52", targetLevel: 6}}}}, true},
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
				SlotsLeft:              tt.fields.SlotsLeft,
			}
			if got := game.isLevelUpLeechAvailable(); got != tt.want {
				t.Errorf("isLevelUpLeechAvailable() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGame_isLevelUpHostAvailable(t *testing.T) {
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
	tests := []struct {
		name   string
		fields fields
		want   bool
	}{
		{"Test not enough food", fields{FoodManager: FoodManagement{FoodPriority: 8}}, false},
		{"Test not enough resources", fields{Level: 4, MessageManager: MessagesManagement{waitingForLevelUp: true},
			LevelUpResources: map[int]Inventory{4: {Player: 4, Linemate: 0, Deraumere: 1, Sibur: 1, Mendiane: 0, Phiras: 0, Thystame: 0}}},
			false},
		{"Test sufficient resources", fields{Level: 4, MessageManager: MessagesManagement{waitingForLevelUp: true},
			LevelUpResources: map[int]Inventory{4: {Player: 4, Linemate: 0, Deraumere: 0, Sibur: 0, Mendiane: 0, Phiras: 0, Thystame: 0}}},
			true},
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
			if got := game.isLevelUpHostAvailable(); got != tt.want {
				t.Errorf("isLevelUpHostAvailable() = %v, want %v", got, tt.want)
			}
		})
	}
}
