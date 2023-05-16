#include <TaskScheduler.h>

#include "Config.h"
#include "Movement.h"
#include "Sensors.h"

Scheduler runner;
void executeMovement();

Task executeMovementTask(SENSOR_CHECK_INTERVAL, TASK_FOREVER, &executeMovement);
#ifdef DEBUG
Task sensorPrintDebugTask(1000, TASK_FOREVER, &sensorPrintDebug);
#endif


void setup() {
	Serial.begin(9600);
	delay(2000);

	pinMode(LED, OUTPUT);
	pinMode(NORTH_SWITCH, OUTPUT);
	pinMode(SOUTH_SWITCH, OUTPUT);
	pinMode(EAST_SWITCH, OUTPUT);
	pinMode(WEST_SWITCH, OUTPUT);
	pinMode(NORTH_LIMIT_SWITCH, INPUT_PULLUP);
	pinMode(SOUTH_LIMIT_SWITCH, INPUT_PULLUP);
	
	runner.init();
	runner.addTask(executeMovementTask);
#ifdef DEBUG
	runner.addTask(sensorPrintDebugTask);
#endif
	executeMovementTask.enable();
}

void loop() {
	runner.execute();
}
