#include "Sensors.h"
#include "Movement.h"
#include "RunningAverage.h"
#include <ArduinoMqttClient.h>

#ifdef WIND_MQTT
extern MqttClient mqtt;
int RECEIVED_WIND;
#endif

RunningAverage windData(WIND_SAMPLES);

struct brightness readSensors() {
	brightness brightnessLevel;
	brightnessLevel.north = analogRead(NORTH_SENSOR);
	brightnessLevel.south = analogRead(SOUTH_SENSOR);
	brightnessLevel.east = analogRead(EAST_SENSOR);
	brightnessLevel.west = analogRead(WEST_SENSOR);
	return brightnessLevel;
}

#ifdef WIND_MQTT
/*
	MQTT wind callback
*/
void windMqttCallback(int messageLength) {
	while (mqtt.available()) {
		RECEIVED_WIND = mqtt.parseInt();
#ifdef DEBUG
		Serial.print("windMqttCallback:\tread ");
		Serial.println(RECEIVED_WIND);
#endif
	}
}
#endif

uint8_t readWind() {
	#ifdef WIND_ANALOG
	return analogRead(WIND_SENSOR);
	#endif
	#ifdef WIND_MQTT
	return RECEIVED_WIND;
	#endif
}

bool isDark(brightness levels) {
	return (levels.north + levels.south + levels.east + levels.west) < DARK_LIMIT;
}

void windCheck() {
#ifdef DEBUG
	Serial.println("windCheck:\tcalled");
#endif
	int wind = readWind();
	windData.addValue(wind);

	if (windData.getCount() < WIND_SAMPLES) {
#ifdef DEBUG
	Serial.println("windCheck:\ttoo few values");
#endif
		return;
	}
	if (windData.getMinInBuffer() > WIND_LIMIT) {
#ifdef DEBUG
	Serial.print("windCheck:\tALERT!!!1!1 => min: ");
	Serial.println(windData.getMinInBuffer());
#endif
		windData.clear();
		goHome();
	}
}

#ifdef DEBUG
void sensorPrintDebug() {
	brightness sensors = readSensors();
	Serial.print("sensorPrintDebug:\tN=");
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