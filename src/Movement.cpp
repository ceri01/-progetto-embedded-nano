#include "Movement.h"

#include <TM1638plus.h>

#ifdef DEBUG
#include <MemoryFree.h>
#endif

bool NORTH_LIMIT_REACHED = false;
bool SOUTH_LIMIT_REACHED = false;
bool EAST_LIMIT_REACHED = false;
bool WEST_LIMIT_REACHED = false;

extern Scheduler runner;
extern Scheduler hpRunner;
extern Task executeMovementTask;
extern Task goHomeDarkFeedbackTask;
extern Task goHomeWindFeedbackTask;
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

void motorMove(Direction direction, int period) {
	uint8_t digitalPin = static_cast<int>(direction);
	digitalWrite(digitalPin, LOW);

#ifdef DEBUG
	Serial.print("motorMove:\tMoving motor on PIN ");
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
	Serial.print("motorMove:\tFree memory:\t");
	Serial.println(freeMemory());
#endif

	// Turn on corresponding LED on TM1638
	tm.setLED(directionToLEDNum(direction), HIGH);
}

void motorMoveFeedback() {
#ifdef DEBUG
	Serial.print("motorMoveFeedback:\tcalled with ID=");
	Serial.println(hpRunner.currentTask().getId());
#endif

	// Get LTS memory
	Direction& direction = *((Direction*) hpRunner.currentLts());
	uint8_t digitalPin = static_cast<int>(direction);

	// Read limit switches
	if (direction == Direction::North) {
		NORTH_LIMIT_REACHED = digitalRead(NORTH_LIMIT_SWITCH);
		SOUTH_LIMIT_REACHED = false;
	} else if (direction == Direction::South) {
		SOUTH_LIMIT_REACHED = digitalRead(SOUTH_LIMIT_SWITCH);
		NORTH_LIMIT_REACHED = false;
	} else if (direction == Direction::East) {
		EAST_LIMIT_REACHED = digitalRead(EAST_LIMIT_SWITCH);
		WEST_LIMIT_REACHED = false;
	} else if (direction == Direction::West) {
		WEST_LIMIT_REACHED = digitalRead(WEST_LIMIT_SWITCH);
		EAST_LIMIT_REACHED = false;
	}

#ifdef DEBUG
	Serial.print("motorMoveFeedback:\tlimit north: ");
	Serial.print(NORTH_LIMIT_REACHED);
	Serial.print("\tlimit south: ");
	Serial.print(SOUTH_LIMIT_REACHED);
	Serial.print("\tlimit east: ");
	Serial.print(EAST_LIMIT_REACHED);
	Serial.print("\tlimit west: ");
	Serial.println(WEST_LIMIT_REACHED);
#endif

	// Actually shut down the motor
	digitalWrite(digitalPin, HIGH);

	// Shut down the LED
	tm.setLED(directionToLEDNum(direction), LOW);

	// Disable the task and free up the memory
	Task *currentTask = hpRunner.getCurrentTask();
	hpRunner.deleteTask(*currentTask);
	free(currentTask);
	free(&direction);

#ifdef DEBUG
	Serial.print("motorMoveFeedback:\tmemory free:");
	Serial.println(freeMemory());
#endif
}

bool executeMovementOnEnable() {
	NORTH_LIMIT_REACHED = false;
	SOUTH_LIMIT_REACHED = false;
	EAST_LIMIT_REACHED = false;
	WEST_LIMIT_REACHED = false;
	return true;
}

void executeMovement() {
#ifdef DEBUG
	Serial.println("executeMovement:\tcalled");
#endif

	const brightness data = readSensors();

	// If it's dark, go home and have a good night sleep
	if (isDark(data)) {
		goHomeDark();
		return;
	}

	const int vertical = data.north - data.south;
	const int horizontal = data.east - data.west;

	if (vertical > EPSILON && !NORTH_LIMIT_REACHED) {
		motorMove(Direction::North, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("executeMovement:\tMoving north.");
#endif
	} else if (vertical < -EPSILON && !SOUTH_LIMIT_REACHED) {
		motorMove(Direction::South, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("executeMovement:\tMoving south.");
#endif
	} else if (horizontal > EPSILON && !EAST_LIMIT_REACHED) {
		motorMove(Direction::East, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("executeMovement:\tMoving east.");
#endif
	} else if (horizontal < -EPSILON && !WEST_LIMIT_REACHED) {
		motorMove(Direction::West, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("executeMovement:\tMoving west.");
#endif
	}
}

void goHomeDark() {
#ifdef DEBUG
	Serial.println("goHomeDark:\tGOING HOME");
#endif

	// Disable the 'auto mode' tasks
	executeMovementTask.disable();
	windCheckTask.disable();
	displaySensorsTask.disable();

	// Spawn the goHomeDarkFeedback task
	NORTH_LIMIT_REACHED = false;
	EAST_LIMIT_REACHED = false;
	goHomeDarkFeedbackTask.enable();

	// Display text
	tm.displayText("SLEEP... ");
}

void goHomeDarkFeedback() {
#ifdef DEBUG
	Serial.println("goHomeDarkFeedback:\tcalled");
#endif
	if (!NORTH_LIMIT_REACHED) {
#ifdef DEBUG
		Serial.println("goHomeDarkFeedback:\tmoving north...");
#endif
		motorMove(Direction::North, GO_HOME_MOVEMENT_TIME);
	} else if (!EAST_LIMIT_REACHED) {
#ifdef DEBUG
		Serial.println("goHomeDarkFeedback:\tmoving east...");
#endif
		motorMove(Direction::East, GO_HOME_MOVEMENT_TIME);
	} else {
#ifdef DEBUG
		Serial.println("goHomeDarkFeedback:\tself-destroy");
#endif
		goHomeDarkFeedbackTask.disable();

		// Re-enable the 'auto mode' tasks
		executeMovementTask.enableDelayed(HOME_SLEEP_TIME);
		windCheckTask.enableDelayed(HOME_SLEEP_TIME);
		displaySensorsTask.enableDelayed(HOME_SLEEP_TIME);

		// Reset limit sensor
		NORTH_LIMIT_REACHED = false;
		EAST_LIMIT_REACHED = false;
	}
}

void goHomeWind() {
#ifdef DEBUG
	Serial.println("goHomeWind:\tGOING HOME");
#endif

	// Disable the 'auto mode' tasks
	executeMovementTask.disable();
	windCheckTask.disable();
	displaySensorsTask.disable();

	// Spawn the goHomeWindFeedback task
	NORTH_LIMIT_REACHED = false;
	goHomeWindFeedbackTask.enable();

	// Display text and alarm LEDs
	tm.displayText("WINdStOP");
	for (int i = 4; i < 8; i++) {
		tm.setLED(i, true);
	}
}


void goHomeWindFeedback() {
#ifdef DEBUG
	Serial.println("goHomeWindFeedback:\tcalled");
#endif
	if (!NORTH_LIMIT_REACHED) {
#ifdef DEBUG
		Serial.println("goHomeWindFeedback:\tmoving north...");
#endif
		motorMove(Direction::North, GO_HOME_MOVEMENT_TIME);
	} else {
#ifdef DEBUG
		Serial.println("goHomeWindFeedback:\tself-destroy");
#endif
		goHomeWindFeedbackTask.disable();

		// Re-enable the 'auto mode' tasks
		executeMovementTask.enableDelayed(WIND_SLEEP_TIME);
		windCheckTask.enableDelayed(WIND_SLEEP_TIME);
		displaySensorsTask.enableDelayed(WIND_SLEEP_TIME);

		// Reset limit sensor
		NORTH_LIMIT_REACHED = false;
	}
}
