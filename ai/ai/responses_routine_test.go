package ai

import (
	"testing"
	"time"
	"zappy_ai/network"
)

func helperTest_awaitResponseToCommand(channel chan<- bool, value bool, foodChannel chan<- int) {
	time.Sleep(10 * time.Millisecond)
	channel <- value
	foodChannel <- 2
}

func TestGame_awaitResponseToCommand(t *testing.T) {
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
		{"Test False", fields{Socket: network.ServerConn{ResponseFeedbackChannel: make(chan bool)}, FoodManager: FoodManagement{FoodPriority: 2, InputFoodChannel: make(chan int)}}, false},
		{"Test True", fields{Socket: network.ServerConn{ResponseFeedbackChannel: make(chan bool)}, FoodManager: FoodManagement{FoodPriority: 2, InputFoodChannel: make(chan int)}}, true},
		{"Test closed channel", fields{Socket: network.ServerConn{ResponseFeedbackChannel: make(chan bool)}, FoodManager: FoodManagement{FoodPriority: 2, InputFoodChannel: make(chan int)}, Level: 1}, true},
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
			go helperTest_awaitResponseToCommand(game.Socket.ResponseFeedbackChannel, tt.want, game.FoodManager.InputFoodChannel)
			if game.Level == 1 {
				close(game.Socket.ResponseFeedbackChannel)
				tt.want = false
			}
			if got := game.awaitResponseToCommand(); got != tt.want {
				t.Errorf("awaitResponseToCommand() = %v, want %v", got, tt.want)
			}
		})
	}
}
