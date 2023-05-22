#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>

#define DEBUG
// #define ENABLE_COMMUNICATION

// Task scheduler configuration
#define _TASK_PRIORITY
// #define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL
#define _TASK_SLEEP_ON_IDLE_RUN
//#define _TASK_SELF_DESTRUCT
#define _TASK_LTS_POINTER
#include <TaskSchedulerDeclarations.h>

// Pin configuration

#define NORTH_SENSOR A0
#define SOUTH_SENSOR A3
#define EAST_SENSOR A1
#define WEST_SENSOR A2
#define WIND_SENSOR A4
#define LED 13

#define NORTH_SWITCH 2
#define SOUTH_SWITCH 3
#define EAST_SWITCH 4
#define WEST_SWITCH 5
#define NORTH_LIMIT_SWITCH 6
#define SOUTH_LIMIT_SWITCH 7

#define EPSILON 30
#define DARK_LIMIT 50
#define WIND_LIMIT 100

#define MOTOR_MOVEMENT_TIME 1 * TASK_SECOND
#define GO_HOME_MOVEMENT_TIME 30 * TASK_SECOND

#define HOME_SLEEP_TIME 2 * TASK_HOUR

#define SENSOR_CHECK_INTERVAL 5 * TASK_SECOND
#define COMMUNICATION_INTERVAL 10 * TASK_SECOND

#endif
