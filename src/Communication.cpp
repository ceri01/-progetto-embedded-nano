#include "Communication.h"
#include <ArduinoJson.h>
#include <StreamUtils.h>

// wrap serial into eccSerial
HammingStream<7, 4> eccSerial(Serial);

void sendData() {
	// read data from sensors
	brightness brightness = readSensors();
	
	// serialize data to JSON
	StaticJsonDocument<256> jsonData;
	jsonData["north"] = brightness.north;
	jsonData["south"] = brightness.south;
	jsonData["east"] = brightness.east;
    jsonData["west"] = brightness.west;
	jsonData["wind"] = readWind();

	serializeJson(jsonData, eccSerial);
}