#include "Movement.h"

/*
	Move a motor in a given direction for a given time (in milliseconds).
	Then, return true if the limit switch was activated, false otherwise.
*/ 
bool motorMove(Direction direction, int period) {
	uint8_t digitalPin = static_cast<int>(direction);
	
	digitalWrite(digitalPin, HIGH);
	delay(period);

	bool ret = false;
	if (direction == Direction::North) {
		ret = digitalRead(NORTH_LIMIT_SWITCH);
	} else if (direction == Direction::South) {
		ret = digitalRead(SOUTH_LIMIT_SWITCH);
	}
	digitalWrite(digitalPin, LOW);
	return ret;
}

/*
	Move motors to maximize total brightness.
*/
void executeMovement() {
	brightness data = readSensors();
	int vertical = data.north - data.south;
	int horizontal = data.east - data.west;

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
	while (!motorMove(Direction::South, MOTOR_MOVEMENT_TIME)) { }
};