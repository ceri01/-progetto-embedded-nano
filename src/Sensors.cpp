#include "Sensors.h"

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

bool isDark(brightness levels) {
	return levels.north + levels.south + levels.east + levels.west < DARK_LIMIT;
}

#ifdef DEBUG
void sensorPrintDebug() {
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
}
#endif