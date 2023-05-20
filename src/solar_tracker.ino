#include "Config.h"

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "Movement.h"
#include "Sensors.h"
#include "Communication.h"

Scheduler runner, hpRunner;

Task executeMovementTask(SENSOR_CHECK_INTERVAL, TASK_FOREVER, &executeMovement, &runner);
Task communicationTask(COMMUNICATION_INTERVAL, TASK_FOREVER, &sendData, &runner);
Task motorFeedbackTask(TASK_MILLISECOND, TASK_ONCE, &motorMoveFeedback, &hpRunner);
#ifdef DEBUG
Task sensorPrintDebugTask(1000, TASK_FOREVER, &sensorPrintDebug, &runner);
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
	runner.setHighPriorityScheduler(&hpRunner);

	executeMovementTask.enable();
	sensorPrintDebugTask.enable();
}

void loop() {
	runner.execute();
}
