#include "Config.h"

#include <TM1638plus.h>
#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "Movement.h"
#include "Sensors.h"
#include "TM_Module.h"
#ifdef ENABLE_COMMUNICATION
#include "Communication.h"
#endif

// Scheduler declaration
Scheduler runner, hpRunner;

// Tasks declaration
Task executeMovementTask(SENSOR_CHECK_INTERVAL, TASK_FOREVER, &executeMovement);
#ifdef ENABLE_COMMUNICATION
Task communicationTask(COMMUNICATION_INTERVAL, TASK_FOREVER, &sendData);
#endif
#ifdef DEBUG
Task sensorPrintDebugTask(1000, TASK_FOREVER, &sensorPrintDebug);
#endif
Task goHomeFeedbackTask(GO_HOME_MOVEMENT_TIME + TASK_SECOND, TASK_FOREVER, &goHomeFeedback);
Task windCheckTask(WIND_CHECK_INTERVAL, TASK_FOREVER, &windCheck);
Task buttonsCheckTask(WIND_CHECK_INTERVAL, TASK_FOREVER, &buttonsCheck);
Task displaySensorsTask(DISPLAY_CYCLE_INTERVAL, TASK_FOREVER, &displaySensors);

// TM1638plus declatarion
TM1638plus tm(TM_STROBE, TM_CLOCK, TM_DIO, TM_HIGH_FREQ);

void setup() {
	// init serial with baud rate
	Serial.begin(SERIAL_BAUD_RATE);

	// init pins
	pinMode(LED, OUTPUT);
	pinMode(NORTH_SWITCH, OUTPUT);
	pinMode(SOUTH_SWITCH, OUTPUT);
	pinMode(EAST_SWITCH, OUTPUT);
	pinMode(WEST_SWITCH, OUTPUT);
	pinMode(NORTH_LIMIT_SWITCH, INPUT);
	pinMode(SOUTH_LIMIT_SWITCH, INPUT);

	// Init display
	tm.displayBegin();
	tm.displayText("--------");

	// Reset all relays
	for (const auto direction : ALL_DIRECTIONS) {
		uint8_t digitalPin = static_cast<int>(direction);
		digitalWrite(digitalPin, HIGH);
	}
	
	// init scheduler
	runner.init();

	// set priority of schedulers
	runner.setHighPriorityScheduler(&hpRunner);

	runner.addTask(executeMovementTask);
#ifdef ENABLE_COMMUNICATION
	runner.addTask(communicationTask);
#endif
#ifdef DEBUG
	runner.addTask(sensorPrintDebugTask);
#endif
	// adding tasks
	runner.addTask(goHomeFeedbackTask);
	runner.addTask(windCheckTask);
	runner.addTask(buttonsCheckTask);
	runner.addTask(displaySensorsTask);

	// enable tasks
	executeMovementTask.enable();
#ifdef ENABLE_COMMUNICATION
	communicationTask.enable();
#endif
#ifdef DEBUG
	sensorPrintDebugTask.enable();
#endif
	windCheckTask.enable();
	buttonsCheckTask.enable();
	displaySensorsTask.enable();
}

void loop() {
	runner.execute();
}
