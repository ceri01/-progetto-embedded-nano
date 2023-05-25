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

struct brightness readSensors();

uint8_t readWind();

bool isDark(brightness levels);

void windCheck();

#ifdef DEBUG
void sensorPrintDebug();
#endif

#endif
