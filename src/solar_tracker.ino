#include "Config.h"

#define DEBUG

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "Movement.h"
#include "Sensors.h"
#ifdef ENABLE_COMMUNICATION
#include "Communication.h"
#endif

Scheduler runner, hpRunner;

Task executeMovementTask(SENSOR_CHECK_INTERVAL, TASK_FOREVER, &executeMovement);
#ifdef ENABLE_COMMUNICATION
Task communicationTask(COMMUNICATION_INTERVAL, TASK_FOREVER, &sendData);
#endif
#ifdef DEBUG
Task sensorPrintDebugTask(1000, TASK_FOREVER, &sensorPrintDebug);
#endif
Task goHomeTask(GO_HOME_MOVEMENT_TIME + TASK_SECOND, TASK_FOREVER, &goHomeFeedback);
Task windCheckTask(WIND_CHECK_INTERVAL, TASK_FOREVER, &windCheck);

void setup() {
	Serial.begin(9600, SERIAL_7N1);

	pinMode(LED, OUTPUT);
	pinMode(NORTH_SWITCH, OUTPUT);
	pinMode(SOUTH_SWITCH, OUTPUT);
	pinMode(EAST_SWITCH, OUTPUT);
	pinMode(WEST_SWITCH, OUTPUT);
	pinMode(NORTH_LIMIT_SWITCH, INPUT);
	pinMode(SOUTH_LIMIT_SWITCH, INPUT);

	// Reset all relays
	for (const auto direction : ALL_DIRECTIONS) {
		uint8_t digitalPin = static_cast<int>(direction);
		digitalWrite(digitalPin, LOW);
	}
	
	runner.init();
	runner.setHighPriorityScheduler(&hpRunner);

	runner.addTask(executeMovementTask);
#ifdef ENABLE_COMMUNICATION
	runner.addTask(communicationTask);
#endif
#ifdef DEBUG
	runner.addTask(sensorPrintDebugTask);
#endif
	runner.addTask(goHomeTask);

	executeMovementTask.enable();
#ifdef ENABLE_COMMUNICATION
	communicationTask.enable();
#endif
#ifdef DEBUG
	sensorPrintDebugTask.enable();
#endif DEBUG
}

void loop() {
	runner.execute();
}
