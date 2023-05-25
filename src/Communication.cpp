#include "Communication.h"
#include <ArduinoJson.h>
#include <StreamUtils.h>


HammingStream<7, 4> eccSerial(Serial);

void sendData() {
	brightness brightness = readSensors();
	StaticJsonDocument<128> jsonData;
	jsonData["north"] = brightness.north;
	jsonData["south"] = brightness.south;
	jsonData["east"] = brightness.east;
	jsonData["west"] = brightness.west;
	jsonData["wind"] = readWind();

	serializeJson(jsonData, eccSerial);
}