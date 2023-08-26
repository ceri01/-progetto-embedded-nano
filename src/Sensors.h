#ifndef _SENSORS_H
#define _SENSORS_H

#include "Config.h"

/*
	Struct to describe each sensor value
*/
struct brightness {
	int north;
	int south;
	int east;
	int west;
};

/*
	Read and return the data provided from light sensors. 
*/
struct brightness readSensors();

/*
	Read and return the data provided from wind sensor
*/
uint8_t readWind();

#ifdef WIND_MQTT
/*
	MQTT wind callback
*/
void windMqttCallback(String &topic, String &payload);
#endif

/*
	Check if brightness is too low (if is night)
*/
bool isDark(brightness levels);

/*
	Check wind vhighalue and enable sleep mode if it's too 
*/
void windCheck();

#ifdef DEBUG
/*
	Print sensors data to debug  	
*/
void sensorPrintDebug();
#endif

#endif
