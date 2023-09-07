#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include "Config.h"
#include "Sensors.h"

// Define the four directions of movement
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

/*
    Return LED number corresponding to given Direction  
*/
uint8_t directionToLEDNum(Direction direction);

/*
	Move a motor in a given direction for a given time (in milliseconds).
*/
void motorMove(Direction, int);

/*
    Performs checks and operatios after every motorMove call:
    - check limit switches
    - Shut down motors
    - Shut down LED
    - Disable current task and free memory
*/
void motorMoveFeedback();

/*
    Reset limit switches
*/
bool executeMovementOnEnable();

/*
	Move motors to maximize total brightness.
*/
void executeMovement();

/*
	Move panel to the default position (vertical and horizontal)
*/
void goHomeDark();

/*
	Periodically check if the 'going home' has reached the south and east limit
*/
void goHomeDarkFeedback();

/*
    Move panel to the default position (vertical)
*/
void goHomeWind();

/*
	Periodically check if the 'going home' has reached the south limit
*/
void goHomeWindFeedback();

#endif
