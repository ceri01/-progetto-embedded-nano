#include "Movement.h"

#include <TM1638plus.h>

#ifdef DEBUG
#include <MemoryFree.h>
#endif

bool NORTH_LIMIT_REACHED = false;
bool SOUTH_LIMIT_REACHED = false;

extern Scheduler runner;
extern Scheduler hpRunner;
extern Task executeMovementTask;
extern Task goHomeFeedbackTask;
extern Task windCheckTask;
extern Task displaySensorsTask;

extern TM1638plus tm;

uint8_t directionToLEDNum(Direction direction) {
	switch (direction) {
		case Direction::North:
			return 0;

		case Direction::South:
			return 1;
		
		case Direction::East:
			return 2;

		case Direction::West:
			return 3;
	}
}

/*
	Move a motor in a given direction for a given time (in milliseconds).
*/
void motorMove(Direction direction, int period) {
	uint8_t digitalPin = static_cast<int>(direction);
	digitalWrite(digitalPin, HIGH);

#ifdef DEBUG
	Serial.print("Moving motor on PIN ");
	Serial.print(digitalPin);
	Serial.print(" for ");
	Serial.print(period);
	Serial.println(" milliseconds");
#endif

	Task *motorFeedbackTask = new Task(TASK_MILLISECOND, TASK_ONCE, &motorMoveFeedback, &hpRunner, true);
	motorFeedbackTask->delay(period);

	// Allocate the direction LTS object on the heap (not on the stack)
	Direction *directionHeap = (Direction*) malloc(sizeof(Direction));
	*directionHeap = direction;
	motorFeedbackTask->setLtsPointer(directionHeap);

#ifdef DEBUG
	Serial.print("Free memory:\t");
	Serial.println(freeMemory());
#endif

	// Turn on corresponding LED on TM1638
	tm.setLED(directionToLEDNum(direction), HIGH);
}

void motorMoveFeedback() {
#ifdef DEBUG
	Serial.println("motorMoveFeedback:\tcalled");
#endif

	// Get LTS memory
	Direction& direction = *((Direction*) hpRunner.currentLts());
	uint8_t digitalPin = static_cast<int>(direction);

	// Read limit switches
	if (direction == Direction::North) {
		NORTH_LIMIT_REACHED = !digitalRead(NORTH_LIMIT_SWITCH);
		SOUTH_LIMIT_REACHED = false;
	} else if (direction == Direction::South) {
		SOUTH_LIMIT_REACHED = !digitalRead(SOUTH_LIMIT_SWITCH);
		NORTH_LIMIT_REACHED = false;
	}

#ifdef DEBUG
	Serial.print("motorMoveFeedback:\tlimit north: ");
	Serial.print(NORTH_LIMIT_REACHED);
	Serial.print("\tlimit south: ");
	Serial.println(SOUTH_LIMIT_REACHED);
#endif

	// Actually shut down the motor
	digitalWrite(digitalPin, LOW);

	// Shut down the LED
	tm.setLED(directionToLEDNum(direction), LOW);

	// Disable the task and free up the memory
	Task *currentTask = hpRunner.getCurrentTask();
	currentTask->disable();
	free(currentTask);
	free(&direction);
}

/*
	Move motors to maximize total brightness.
*/
void executeMovement() {
#ifdef DEBUG
	Serial.println("executeMovement: called");
#endif

	const brightness data = readSensors();

	// If it's dark, go home and have a good night sleep
	if (isDark(data)) {
		goHome();
		return;
	}

	const int vertical = data.north - data.south;
	const int horizontal = data.east - data.west;

	if (vertical > EPSILON && !NORTH_LIMIT_REACHED) {
		motorMove(Direction::North, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("Moving north.");
#endif
	} else if (vertical < -EPSILON && !SOUTH_LIMIT_REACHED) {
		motorMove(Direction::South, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("Moving south.");
#endif
	} else if (horizontal > EPSILON) {
		motorMove(Direction::East, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("Moving east.");
#endif
	} else if (horizontal < -EPSILON) {
		motorMove(Direction::West, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("Moving west.");
#endif
	}
}

/*
	Move panel to the default position (horizontal)
*/
void goHome() {
#ifdef DEBUG
	Serial.println("GOING HOME");
#endif

	// Disable the 'auto mode' tasks
	executeMovementTask.disable();
	windCheckTask.disable();
	displaySensorsTask.disable();

	// Spawn the goHomeFeedback task
	SOUTH_LIMIT_REACHED = false;
	goHomeFeedbackTask.enable();

	// Display text
	tm.displayText("SLEEP... ");
}

/*
	Periodically check if the 'going home' has reached the south limit
*/
void goHomeFeedback() {
#ifdef DEBUG
	Serial.println("goHomeFeedback called");
#endif
	if (!SOUTH_LIMIT_REACHED) {
#ifdef DEBUG
		Serial.println("goHomeFeedback: moving...");
#endif
		motorMove(Direction::South, GO_HOME_MOVEMENT_TIME);
	} else {
#ifdef DEBUG
		Serial.println("goHomeFeedback: self-destroy");
#endif
		goHomeFeedbackTask.disable();

		// Re-enable the 'auto mode' tasks
		executeMovementTask.enableDelayed(HOME_SLEEP_TIME);
		windCheckTask.enableDelayed(HOME_SLEEP_TIME);
		displaySensorsTask.enableDelayed(HOME_SLEEP_TIME);

		// Reset limit sensor
		SOUTH_LIMIT_REACHED = false;
	}
}