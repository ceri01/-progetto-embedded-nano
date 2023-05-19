#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "Config.h"
#include "Movement.h"
#include "Sensors.h"
#include "Communication.h"

Scheduler runner;
void executeMovement();

Task executeMovementTask(SENSOR_CHECK_INTERVAL, TASK_FOREVER, &executeMovement);
Task communicationTask(COMMUNICATION_INTERVAL, TASK_FOREVER, &sendData);
#ifdef DEBUG
Task sensorPrintDebugTask(1000, TASK_FOREVER, &sensorPrintDebug);
#endif

void setup() {
	Serial.begin(9600, SERIAL_7N1);

	pinMode(LED, OUTPUT);
	pinMode(NORTH_SWITCH, OUTPUT);
	pinMode(SOUTH_SWITCH, OUTPUT);
	pinMode(EAST_SWITCH, OUTPUT);
	pinMode(WEST_SWITCH, OUTPUT);
	pinMode(NORTH_LIMIT_SWITCH, INPUT_PULLUP);
	pinMode(SOUTH_LIMIT_SWITCH, INPUT_PULLUP);
	
	runner.init();
	runner.addTask(executeMovementTask);
	runner.addTask(communicationTask);
#ifdef DEBUG
	runner.addTask(sensorPrintDebugTask);
#endif
	executeMovementTask.enable();
}

void loop() {
	runner.execute();
}
