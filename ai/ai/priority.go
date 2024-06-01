package ai

// levelUpResources is a map of TileItem -> ints maps containing the necessary resources for level ups
// This is a pre-initialized placeholder for creating the game struct
var levelUpResources = map[int]Inventory{
	1: {Player: 1, Linemate: 1, Deraumere: 0, Sibur: 0, Mendiane: 0, Phiras: 0, Thystame: 0},
	2: {Player: 2, Linemate: 1, Deraumere: 1, Sibur: 1, Mendiane: 0, Phiras: 0, Thystame: 0},
	3: {Player: 2, Linemate: 2, Deraumere: 0, Sibur: 1, Mendiane: 0, Phiras: 2, Thystame: 0},
	4: {Player: 4, Linemate: 1, Deraumere: 1, Sibur: 2, Mendiane: 0, Phiras: 1, Thystame: 0},
	5: {Player: 4, Linemate: 1, Deraumere: 2, Sibur: 1, Mendiane: 3, Phiras: 0, Thystame: 0},
	6: {Player: 6, Linemate: 1, Deraumere: 2, Sibur: 3, Mendiane: 0, Phiras: 1, Thystame: 0},
	7: {Player: 6, Linemate: 2, Deraumere: 2, Sibur: 2, Mendiane: 2, Phiras: 2, Thystame: 1},
}

// totalResourcesRequired is an Inventory containing the total resources left to collect to reach level 8
// This is a pre-initialized placeholder for creating the game struct
var totalResourcesRequired = Inventory{Linemate: 9, Deraumere: 8, Sibur: 10, Mendiane: 5, Phiras: 6, Thystame: 1}

func (game Game) resourceCollected(item TileItem) {
	if item == Player {
		return
	}
	if item == Food {
		game.FoodChannel <- 1
		return
	}
	for i := game.Level; i < 8; i++ {
		if game.LevelUpResources[i][item] > 0 {
			game.LevelUpResources[i][item]--
			break
		}
	}
	if game.TotalResourcesRequired[item] > 0 {
		game.TotalResourcesRequired[item]--
	}
}

// removeLevelUpResources removes the resources required for the previous level up
// removeLevelUpResources also updates the total resources required to reach level 8
func (game Game) removeLevelUpResources() {
	for i := 1; i < game.Level; i++ {
		for key, value := range game.LevelUpResources[i] {
			if key != Player {
				game.TotalResourcesRequired[key] -= value
				game.LevelUpResources[i][key] = 0
			}
		}
	}
}

// isResourceRequired checks if the resource is going to be needed at some point in the future.
// isResourceRequired always return false for a Player and true for Food
func (game Game) isResourceRequired(item TileItem) bool {
	if item == Player {
		return false
	}
	if item == Food {
		return true
	}
	return game.TotalResourcesRequired[item] > 0
}
