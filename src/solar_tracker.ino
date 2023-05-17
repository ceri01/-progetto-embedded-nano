#include <TaskScheduler.h>

#define NORTH_SENSOR A0
#define SOUTH_SENSOR A3
#define EAST_SENSOR A1
#define WEST_SENSOR A2
#define LED 13

#define NORTH_SWITCH 2
#define SOUTH_SWITCH 3
#define EAST_SWITCH 4
#define WEST_SWITCH 5
#define NORTH_LIMIT_SWITCH 6
#define SOUTH_LIMIT_SWITCH 7

#define EPSILON 30
#define CORRECTION 2000

#define DEBUG

/*
	Struct to describe each sensor value
*/
struct brightness {
	int north;
	int south;
	int east;
	int west;
};

enum Direction { North = NORTH_SWITCH, South = SOUTH_SWITCH, East = EAST_SWITCH, West = WEST_SWITCH};

/*
	Read and return the data provided from light sensors. 
*/
struct brightness readSensors() {
	brightness brightnessLevel;
	brightnessLevel.north = analogRead(NORTH_SENSOR);
	brightnessLevel.south = analogRead(SOUTH_SENSOR);
	brightnessLevel.east = analogRead(EAST_SENSOR);
	brightnessLevel.west = analogRead(WEST_SENSOR);
	return brightnessLevel;
}

/*
	Move motors to maximize total brightness.
*/
void executeMovement() {
	brightness data = readSensors();
	int vertical = data.north - data.south;
	int horizontal = data.east - data.west;

	if (vertical > EPSILON) {
		motorMove(Direction::North, CORRECTION);
#ifdef DEBUG
		Serial.println("Moving north.");
#endif
	} else if (vertical < -EPSILON) {
		motorMove(Direction::South, CORRECTION);
#ifdef DEBUG
		Serial.println("Moving south.");
#endif
	}

	if (horizontal > EPSILON) {
		motorMove(Direction::East, CORRECTION);
#ifdef DEBUG
		Serial.println("Moving east.");
#endif
	} else if (horizontal < -EPSILON) {
		motorMove(Direction::West, CORRECTION);
#ifdef DEBUG
		Serial.println("Moving west.");
#endif
	}
}

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

void setup() {
	Serial.begin(9600);
	delay(2000);

	pinMode(LED, OUTPUT);
	pinMode(NORTH_SWITCH, OUTPUT);
	pinMode(SOUTH_SWITCH, OUTPUT);
	pinMode(EAST_SWITCH, OUTPUT);
	pinMode(WEST_SWITCH, OUTPUT);
	pinMode(NORTH_LIMIT_SWITCH, INPUT_PULLUP);
	pinMode(SOUTH_LIMIT_SWITCH, INPUT_PULLUP);
}

void loop() {
#ifdef DEBUG
	brightness sensors = readSensors();
	Serial.print("N=");
	Serial.print(sensors.north);
	Serial.print("\tS=");
	Serial.print(sensors.south);
	Serial.print("\tE=");
	Serial.print(sensors.east);
	Serial.print("\tW=");
	Serial.print(sensors.west);
	Serial.println();
#endif

	executeMovement();
	delay(1000);
}
