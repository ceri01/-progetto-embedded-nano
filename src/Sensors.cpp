#include "Sensors.h"
#include "Movement.h"
#include "RunningAverage.h"

RunningAverage windData(WIND_SAMPLES);

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

uint8_t readWind() {
	return analogRead(WIND_SENSOR);
}

bool isDark(brightness levels) {
	return (levels.north + levels.south + levels.east + levels.west) < DARK_LIMIT;
}

void windCheck() {
#ifdef DEBUG
	Serial.println("windCheck called");
#endif
	int wind = readWind();
	windData.addValue(wind);

	if (windData.getCount() < WIND_SAMPLES) {
#ifdef DEBUG
	Serial.println("windCheck: too few values");
#endif
		return;
	}
	if (windData.getMinInBuffer() > WIND_LIMIT) {
#ifdef DEBUG
	Serial.print("windCheck: ALERT!!!1!1 => min: ");
	Serial.println(windData.getMinInBuffer());
#endif
		windData.clear();
		goHome();
	}
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
	Serial.print("\tWind=");
	Serial.print(readWind());
	Serial.println();
}
#endif