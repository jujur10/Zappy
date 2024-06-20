package ai

import (
	"log"
	"sync"
	"time"
)

const foodLifeTimeIncrement = 126
const playerOutOfFood = -1
const foodMaxPriority = 11
const foodMinPriority = 2

var foodPrioMutex sync.Mutex

func setFoodPriority(foodPrio *int, value int) {
	foodPrioMutex.Lock()
	*foodPrio = value
	foodPrioMutex.Unlock()
}

func getFoodPriority(foodPrio *int) int {
	foodPrioMutex.Lock()
	defer foodPrioMutex.Unlock()
	return *foodPrio
}

// FoodManagementRoutine is meant to run in a goroutine and manage the food
// One lifeTime unit is removed at each turn
// When a whole food item is consumed, the food priority is updated.
// When the player collects food, the collected amount is sent through the channel.
// The lifeTime is incremented by 126 and the priority recomputed.
func FoodManagementRoutine(inputFood <-chan int, foodPrio *int, timeStepChan chan time.Duration) {
	lifeTime := 1260
	consumptionCounter := 0
	timeStep := time.Duration(0)
	if FrequencyCommandAvailable == false {
		lifeTime -= 5
		consumptionCounter += 5
	}
	log.Println("Starting food management routine")
	for { // While true
		select {
		case newFood, ok := <-inputFood:
			if !ok {
				log.Println("Food Management : channel closed, exiting..")
				return
			}
			if newFood < 0 {
				if -newFood < (lifeTime/foodLifeTimeIncrement) ||
					-newFood > ((lifeTime+foodLifeTimeIncrement)/foodLifeTimeIncrement) {
					lifeTime = (-newFood) * foodLifeTimeIncrement
					setFoodPriority(foodPrio, computeFoodPriority(lifeTime))
				}
				break
			}
			lifeTime += foodLifeTimeIncrement * newFood
			setFoodPriority(foodPrio, computeFoodPriority(lifeTime))
		case ts, ok := <-timeStepChan:
			if !ok {
				log.Fatalln("Food Management : channel closed, exiting..")
				return
			}
			timeStep = ts
			//log.Printf("Timestep updated, new timestep is %dms\n", ts.Milliseconds())
		default:

		}
		lifeTime--
		consumptionCounter++
		if lifeTime <= 0 {
			setFoodPriority(foodPrio, playerOutOfFood)
			log.Println("Food Management : player out of food, exiting..")
			return
		}
		if consumptionCounter >= foodLifeTimeIncrement {
			consumptionCounter = 0
			setFoodPriority(foodPrio, computeFoodPriority(lifeTime))
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
