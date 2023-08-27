#include "Config.h"

#include <TM1638plus.h>
#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <MQTT.h>
#include <MemoryFree.h>

#include "Movement.h"
#include "Sensors.h"
#include "TM_Module.h"

// Scheduler declaration
Scheduler runner, hpRunner;

// Tasks declaration
Task executeMovementTask(SENSOR_CHECK_INTERVAL, TASK_FOREVER, &executeMovement);
#ifdef DEBUG
Task sensorPrintDebugTask(1000, TASK_FOREVER, &sensorPrintDebug);
#endif
Task goHomeDarkFeedbackTask(GO_HOME_MOVEMENT_TIME + TASK_SECOND, TASK_FOREVER, &goHomeDarkFeedback);
Task goHomeWindFeedbackTask(GO_HOME_MOVEMENT_TIME + TASK_SECOND, TASK_FOREVER, &goHomeWindFeedback);
Task windCheckTask(WIND_CHECK_INTERVAL, TASK_FOREVER, &windCheck);
Task buttonsCheckTask(WIND_CHECK_INTERVAL, TASK_FOREVER, &buttonsCheck);
Task displaySensorsTask(DISPLAY_CYCLE_INTERVAL, TASK_FOREVER, &displaySensors);

#ifdef WIND_MQTT
// Libraries setup
EthernetClient net;
MQTTClient mqtt;

void ethernetMaintain() {
	Ethernet.maintain();
}

void mqttPoll() {
#ifdef DEBUG
	Serial.println("mqttPoll: polling...");
	Serial.print("mqttPoll:\tmemory free before: ");
	Serial.println(freeMemory());
#endif
	mqtt.loop();
#ifdef DEBUG
	Serial.print("mqttPoll:\tmemory after: ");
	Serial.println(freeMemory());
#endif
}

Task ethernetMaintainTask(10 * TASK_MINUTE, TASK_FOREVER, &ethernetMaintain);
Task mqttPollTask(MQTT_POLL_TIME, TASK_FOREVER, &mqttPoll);
#endif

// TM1638plus declatarion
TM1638plus tm(TM_STROBE, TM_CLOCK, TM_DIO, TM_HIGH_FREQ);

#ifdef WIND_MQTT
// Software reset
void(* reset)(void) = 0;

/*
	Initialize Ethernet
*/
void networkSetup() {
	byte mac[] = MAC_ADDRESS;

	Ethernet.init(10);

#ifdef DEBUG
	Serial.println("Ethernet:\tinitialize with DHCP:");
#endif

	tm.displayText("DHCP.....");
	if (Ethernet.begin(mac) == 0) {
		Serial.println("Ethernet:\tfailed to configure Ethernet using DHCP");
		while (true) {
			reset();
		}
	}

#ifdef DEBUG
	Serial.print("Ethernet:\tmy IP address: ");
	Serial.println(Ethernet.localIP());
#endif
}

/*
	Initialize MQTT
*/
void mqttSetup() {
	mqtt.begin(net);
	mqtt.onMessage(windMqttCallback);

	Serial.print("MQTT:\tconnecting to MQTT");

	mqtt.setHost(MQTT_HOST, MQTT_PORT);
	while (!mqtt.connect("arduino_mega", MQTT_USERNAME, MQTT_PASSWORD)) {
		Serial.print(".");
		delay(200);
	}

#ifdef DEBUG
	Serial.println("\nMQTT: Connected!");
#endif

	mqtt.subscribe(MQTT_TOPIC);
}
#endif  // WIND_MQTT

void setup() {
	// init serial with baud rate
	Serial.begin(SERIAL_BAUD_RATE);

	// init pins
	pinMode(LED, OUTPUT);
	pinMode(NORTH_SWITCH, OUTPUT);
	pinMode(SOUTH_SWITCH, OUTPUT);
	pinMode(EAST_SWITCH, OUTPUT);
	pinMode(WEST_SWITCH, OUTPUT);
	pinMode(NORTH_LIMIT_SWITCH, INPUT_PULLUP);
	pinMode(SOUTH_LIMIT_SWITCH, INPUT_PULLUP);

	// Init display
	tm.displayBegin();
	tm.displayText("--------");

#ifdef WIND_MQTT
	networkSetup();
	mqttSetup();
#endif

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
#ifdef DEBUG
	runner.addTask(sensorPrintDebugTask);
#endif
	// adding tasks
	runner.addTask(goHomeDarkFeedbackTask);
	runner.addTask(goHomeWindFeedbackTask);
	runner.addTask(windCheckTask);
	runner.addTask(buttonsCheckTask);
	runner.addTask(displaySensorsTask);

#ifdef WIND_MQTT
	runner.addTask(ethernetMaintainTask);
	runner.addTask(mqttPollTask);
#endif

	// enable tasks
	executeMovementTask.enable();
#ifdef DEBUG
	sensorPrintDebugTask.enable();
#endif
	windCheckTask.enable();
	buttonsCheckTask.enable();
	displaySensorsTask.enable();

#ifdef WIND_MQTT
	ethernetMaintainTask.enable();
	mqttPollTask.enable();
#endif
}

void loop() {
	runner.execute();
}
