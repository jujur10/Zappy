package ai

import (
	"time"
)

const foodLifeTimeIncrement = 126
const playerOutOfFood = -1
const foodMaxPriority = 11
const foodMinPriority = 2

// FoodManagementRoutine is meant to run in a goroutine and manage the food
// One lifeTime unit is removed at each turn
// When a whole food item is consumed, the food priority is updated.
// When the player collects food, the collected amount is sent through the channel.
// The lifeTime is incremented by 126 and the priority recomputed.
func FoodManagementRoutine(food chan int, timeStepChan chan time.Duration) {
	lifeTime := 1260
	consumptionCounter := 0
	timeStep := time.Duration(0)
	for { // While true
		select {
		case newFood, ok := <-food:
			if !ok {
				return
			}
			if newFood < 0 {
				if -newFood < (lifeTime/foodLifeTimeIncrement) ||
					-newFood > ((lifeTime+foodLifeTimeIncrement)/foodLifeTimeIncrement) {
					lifeTime = (-newFood) * foodLifeTimeIncrement
					food <- computeFoodPriority(lifeTime)
				}
				break
			}
			lifeTime += foodLifeTimeIncrement * newFood
			food <- computeFoodPriority(lifeTime)
		case ts, ok := <-timeStepChan:
			if !ok {
				return
			}
			timeStep = ts
		default:

		}
		lifeTime--
		consumptionCounter++
		if lifeTime <= 0 {
			food <- playerOutOfFood
			return
		}
		if consumptionCounter >= foodLifeTimeIncrement {
			consumptionCounter = 0
			food <- computeFoodPriority(lifeTime)
		}
		time.Sleep(timeStep)
	}
}

// computeFoodPriority returns the priority for the player to collect food.
// It should only be called by the food goroutine.
// It returns 1 if the player has enough food (12 units), 10 if below the safety amount (3 units)
// else the value in between, scaling inversely with the number of food units
func computeFoodPriority(lifeTime int) int {
	if lifeTime >= 12*foodLifeTimeIncrement {
		return foodMinPriority
	}
	if lifeTime <= 3*foodLifeTimeIncrement {
		return foodMaxPriority
	}
	return 14 - (lifeTime / foodLifeTimeIncrement)
}
