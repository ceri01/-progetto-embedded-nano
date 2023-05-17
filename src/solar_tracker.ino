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

#define EPSILON 1
#define DELAY 2000


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
	Read data provided from light sensors.
	This function return a struct that contains four ints 
*/
struct brightness readSensors() {
	brightness brightnessLevel;
	brightnessLevel.north = analogRead(NORTH_SENSOR);
	brightnessLevel.south = analogRead(SOUTH_SENSOR);
	brightnessLevel.east = analogRead(EAST_SENSOR);
	brightnessLevel.west = analogRead(WEST_SENSOR);
	return brightnessLevel;
}

void executeMovement() {
	brightness data = readSensors();
	int vertical = data.north - data.south;
	int horizontal = data.east - data.west;

	if (vertical > EPSILON) {
		motorMove(Direction::North, DELAY);
	} else if (vertical < EPSILON) {
		motorMove(Direction::South, DELAY);
	}

	if (horizontal > EPSILON) {
		motorMove(Direction::East, DELAY);
	} else if (vertical < EPSILON) {
		motorMove(Direction::West, DELAY);
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
	/*
	bool ret;
	ret = motorMove(Direction::North, DELAY);
	Serial.print("N? ");
	Serial.print(ret);

	ret = motorMove(Direction::South, DELAY);
	Serial.print("\tS? ");
	Serial.print(ret);

	ret = motorMove(Direction::East, DELAY);
	Serial.print("\tE? ");
	Serial.print(ret);

	ret = motorMove(Direction::West, DELAY);
	Serial.print("\tW? ");
	Serial.print(ret);

	Serial.println();

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
	*/

	while (!motorMove(Direction::South, 1)) {

	}
	delay(30000);
}
