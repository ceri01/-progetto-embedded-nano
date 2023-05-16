#include "FirmataClient.h"
#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>
#include <TaskScheduler.h>

#define LED 13
#define MOTORE_Z1 2
#define MOTORE_Z2 3
#define MOTORE_Y1 4
#define MOTORE_Y2 5
#define MOTORE_Z1_FEEDBACK 6
#define MOTORE_Z2_FEEDBACK 7

const char* ssid     = "Baracca Observatory";
const char* password = "esaobservatory2019";

const char broker[] = "raspbaracca";
int        port     = 1883;
const char topic[]  = "baracca/lux-ESP01/";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void mqttSend();
Task mqttSendTask(5 * TASK_SECOND, TASK_FOREVER, &mqttSend);

Scheduler runner;

void mqttSend() {
  mqttClient.poll();
  FirmataClient.digitalWrite(LED, HIGH);

  for (int i = 0; i < 6; i++) {
    float voltage = (float) FirmataClient.analogRead(i) * (5.0 / 1023);

    mqttClient.beginMessage(topic + String(i));
    mqttClient.print(voltage);
    mqttClient.endMessage();
    delay(50);
  }

  FirmataClient.digitalWrite(LED, LOW);
	delay(1000);
}

void setup() {
	Serial.begin(57600);
  delay(2000);

	FirmataClient.begin(Serial);
	FirmataClient.setSamplingInterval(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  if (!mqttClient.connect(broker, port)) {
    while (1);
    // TODO: Gestire mancata connessione MQTT
    yield();
  }

  FirmataClient.pinMode(LED, OUTPUT);
  FirmataClient.pinMode(MOTORE_Z1, OUTPUT);
  FirmataClient.pinMode(MOTORE_Z2, OUTPUT);
  FirmataClient.pinMode(MOTORE_Y1, OUTPUT);
  FirmataClient.pinMode(MOTORE_Y2, OUTPUT);
  FirmataClient.pinMode(MOTORE_Z1_FEEDBACK, INPUT);
  FirmataClient.pinMode(MOTORE_Z2_FEEDBACK, INPUT);

  runner.init();
  runner.addTask(mqttSendTask);
  mqttSendTask.enable(); 
}

void loop() {
  /*
  FirmataClient.digitalWrite(MOTORE_Z1, HIGH);
  delay(2000);
  FirmataClient.digitalWrite(MOTORE_Z1, LOW);
  FirmataClient.digitalWrite(MOTORE_Z2, HIGH);
  delay(2000);
  FirmataClient.digitalWrite(MOTORE_Z2, LOW);
  FirmataClient.digitalWrite(MOTORE_Y1, HIGH);
  delay(2000);
  FirmataClient.digitalWrite(MOTORE_Y1, LOW);
  FirmataClient.digitalWrite(MOTORE_Y2, HIGH);
  delay(2000);
  FirmataClient.digitalWrite(MOTORE_Y2, LOW);
  */

 runner.execute();
}