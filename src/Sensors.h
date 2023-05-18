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

#ifdef DEBUG
void sensorPrintDebug();
#endif

#endif
