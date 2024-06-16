package ai

import "zappy_ai/network"

func (game Game) MainLoop() {
	game.Socket.SendCommand(network.LookAround, network.EmptyBody)
	_ = game.awaitResponseToCommand()
	for game.FoodManager.FoodPriority > 0 && game.Level < 8 {

	}
}
