package ai

import (
	"time"
)

const foodLifeTimeIncrement = 126
const playerOutOfFood = -1

func FoodManagementRoutine(food chan int, timeStep time.Duration) {
	lifeTime := 1260
	consumptionCounter := 0
	for true {
		select {
		case newFood, ok := <-food:
			if !ok {
				return
			}
			lifeTime += foodLifeTimeIncrement * newFood
			food <- computeFoodPriority(lifeTime)
		default:

		}
		lifeTime--
		consumptionCounter++
		if lifeTime <= 0 {
			food <- -1
		}
		if consumptionCounter >= foodLifeTimeIncrement {
			consumptionCounter = 0
			food <- computeFoodPriority(lifeTime)
		}
		time.Sleep(timeStep)
	}
}

func computeFoodPriority(lifeTime int) int {
	return 0
}
