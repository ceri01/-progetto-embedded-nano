#include "Movement.h"

Task motorFeedbackTask(TASK_MILLISECOND, TASK_ONCE, &motorMoveFeedback); // non so come chiamarlo

bool SOUTH_LIMIT_REACHED = false;
bool NORTH_LIMIT_REACHED = false;

/*
	Move a motor in a given direction for a given time (in milliseconds).
	Then, return true if the limit switch was activated, false otherwise.
*/

void motorMove(Direction direction, int period) { // togliere parametri
	uint8_t digitalPin = static_cast<int>(direction);
	digitalWrite(digitalPin, HIGH);

#ifdef DEBUG
	Serial.print("Moving motor on PIN ");
	Serial.print(digitalPin);
	Serial.print(" for ");
	Serial.print(period);
	Serial.println(" milliseconds");
#endif

	motorFeedbackTask.setCallback(motorMoveFeedback);
	motorFeedbackTask.delay(period);
	motorFeedbackTask.enable();
}

// questa funzione diventa un task
// (((è già void)))
void motorMoveFeedback() {
#ifdef DEBUG
	Serial.println("motorMoveFeedback called");
#endif

	NORTH_LIMIT_REACHED = digitalRead(NORTH_LIMIT_SWITCH);
	SOUTH_LIMIT_REACHED = digitalRead(SOUTH_LIMIT_SWITCH);

#ifdef DEBUG
	Serial.println("Check feedback");
	Serial.print("Limit north ");
	Serial.println(NORTH_LIMIT_REACHED);
	Serial.print("Limit south ");
	Serial.println(SOUTH_LIMIT_REACHED);
#endif

	// Shut down all motors
	for (const auto direction : ALL_DIRECTIONS) {
		uint8_t digitalPin = static_cast<int>(direction);
		digitalWrite(digitalPin, LOW);
	}
}

/*
	Move motors to maximize total brightness.
*/
void executeMovement() {
	const brightness data = readSensors();
	const int vertical = data.north - data.south;
	const int horizontal = data.east - data.west;

	if (vertical > EPSILON) {
		motorMove(Direction::North, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("Moving north.");
#endif
	} else if (vertical < -EPSILON) {
		motorMove(Direction::South, MOTOR_MOVEMENT_TIME);
#ifdef DEBUG
		Serial.println("Moving south.");
#endif
	}

	if (horizontal > EPSILON) {
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
	while (!SOUTH_LIMIT_REACHED) {
		motorMove(Direction::South, MOTOR_MOVEMENT_TIME);
	};
}
