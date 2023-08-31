#include "Sensors.h"
#include "Movement.h"
#include "RunningAverage.h"

#ifdef WIND_MQTT
float RECEIVED_WIND;
unsigned long RECEIVED_WIND_LAST_TIMESTAMP = 0;
#endif

extern unsigned long timer0_millis;

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
void windMqttCallback(String &topic, String &payload) {
#ifdef DEBUG
	Serial.print("windMqttCallback:\tread ");
	Serial.println(payload);
#endif
	RECEIVED_WIND = payload.toFloat();
	RECEIVED_WIND_LAST_TIMESTAMP = millis();
}

bool readWindOnEnable() {
	// Avoid false positives, for example when switching from manual mode to auto mode 
	RECEIVED_WIND_LAST_TIMESTAMP = millis();
	return true;
}
#endif

float readWind() {
	#ifdef WIND_ANALOG
	return (float) analogRead(WIND_SENSOR);
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

#ifdef WIND_MQTT
	// millis() overflows every ~50 days. This prevents false positives. 
	if (millis() - RECEIVED_WIND_LAST_TIMESTAMP > 24 * TASK_HOUR) {
		// If the last timestamp is REALLY a long time far go, a overflow happened
		RECEIVED_WIND_LAST_TIMESTAMP = millis();
	}
	if (millis() - RECEIVED_WIND_LAST_TIMESTAMP > WIND_MQTT_TIMEOUT) {
#ifdef DEBUG
		Serial.println("readWind:\tALARM!!11!! received wind last timestamp TOO OLD!");
#endif
		goHomeWind();
	}
#endif
	float wind = readWind();
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
		goHomeWind();
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
#ifdef WIND_MQTT
	Serial.print("\tWindLastUpdate=");
	Serial.print(RECEIVED_WIND_LAST_TIMESTAMP);
#endif
	Serial.println();
}
#endif