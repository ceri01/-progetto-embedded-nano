#include <TaskScheduler.h>

#define LED 13
#define MOTORE_Z1 2
#define MOTORE_Z2 3
#define MOTORE_Y1 4
#define MOTORE_Y2 5
#define MOTORE_Z1_FEEDBACK 6
#define MOTORE_Z2_FEEDBACK 7

void setup() {
	Serial.begin(115200);
	delay(2000);

	pinMode(LED, OUTPUT);
	pinMode(MOTORE_Z1, OUTPUT);
	pinMode(MOTORE_Z2, OUTPUT);
	pinMode(MOTORE_Y1, OUTPUT);
	pinMode(MOTORE_Y2, OUTPUT);
	pinMode(MOTORE_Z1_FEEDBACK, INPUT);
	pinMode(MOTORE_Z2_FEEDBACK, INPUT);
}

void loop() {
	digitalWrite(MOTORE_Z1, HIGH);
	delay(2000);
	digitalWrite(MOTORE_Z1, LOW);
	digitalWrite(MOTORE_Z2, HIGH);
	delay(2000);
	digitalWrite(MOTORE_Z2, LOW);
	digitalWrite(MOTORE_Y1, HIGH);
	delay(2000);
	digitalWrite(MOTORE_Y1, LOW);
	digitalWrite(MOTORE_Y2, HIGH);
	delay(2000);
	digitalWrite(MOTORE_Y2, LOW);
}
