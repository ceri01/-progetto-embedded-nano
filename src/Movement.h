#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include "Config.h"
#include "Sensors.h"

enum Direction {
    North = NORTH_SWITCH,
    South = SOUTH_SWITCH,
    East = EAST_SWITCH,
    West = WEST_SWITCH
};

bool motorMove(Direction, int);

void executeMovement();

void goHome();

#endif
