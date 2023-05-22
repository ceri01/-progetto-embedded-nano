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

static const Direction ALL_DIRECTIONS[] = {
    North,
    South,
    East,
    West
};

void motorMove(Direction, int);

void motorMoveFeedback();

void executeMovement();

void goHome();

void goHomeFeedback();

#endif
