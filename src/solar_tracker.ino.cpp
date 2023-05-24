# 1 "/tmp/tmpu319g1_2"
#include <Arduino.h>
# 1 "/home/marco/PlatformIO/Projects/230516-162945-esp01_1m/src/solar_tracker.ino"
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

Scheduler runner, hpRunner;

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

TM1638plus tm(TM_STROBE, TM_CLOCK, TM_DIO, TM_HIGH_FREQ);
void setup();
void loop();
#line 29 "/home/marco/PlatformIO/Projects/230516-162945-esp01_1m/src/solar_tracker.ino"
void setup() {
 Serial.begin(9600, SERIAL_7N1);

 pinMode(LED, OUTPUT);
 pinMode(NORTH_SWITCH, OUTPUT);
 pinMode(SOUTH_SWITCH, OUTPUT);
 pinMode(EAST_SWITCH, OUTPUT);
 pinMode(WEST_SWITCH, OUTPUT);
 pinMode(NORTH_LIMIT_SWITCH, INPUT);
 pinMode(SOUTH_LIMIT_SWITCH, INPUT);


 tm.displayBegin();
 tm.displayText("--------");


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
 runner.addTask(goHomeFeedbackTask);
 runner.addTask(windCheckTask);
 runner.addTask(buttonsCheckTask);

 executeMovementTask.enable();
#ifdef ENABLE_COMMUNICATION
 communicationTask.enable();
#endif
#ifdef DEBUG
 sensorPrintDebugTask.enable();
#endif
 windCheckTask.enable();
 buttonsCheckTask.enable();
}

void loop() {
 runner.execute();
}