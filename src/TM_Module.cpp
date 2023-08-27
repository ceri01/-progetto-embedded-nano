#include <TM1638plus.h>

#include "TM_Module.h"
#include "Config.h"
#include "Sensors.h"

extern Task executeMovementTask;
extern Task windCheckTask;
extern Task goHomeDarkFeedbackTask;
extern Task buttonsCheckTask;
extern Task displaySensorsTask;

extern TM1638plus tm;

bool MANUAL_MODE = false;
bool MODE_BUTTON_PRESSED = false;

// mask used to retrieve byte that represent buttons on TM1638plus
enum BUTTON_MASK {
    NORTH_BUTTON = 0x01,
    SOUTH_BUTTON = 0x02,
    EAST_BUTTON = 0x04,
    WEST_BUTTON = 0x08,
    MODE_BUTTON = 0x80,
};

uint8_t SENSOR_DISPLAY_CYCLE = 0x00;

void buttonsCheck() {
    const uint8_t buttons = tm.readButtons();

    // Parse button values
    const uint8_t mode = buttons & MODE_BUTTON;
    const uint8_t north = buttons & NORTH_BUTTON;
    const uint8_t south = buttons & SOUTH_BUTTON;
    const uint8_t east = buttons & EAST_BUTTON;
    const uint8_t west = buttons & WEST_BUTTON;

#ifdef DEBUG
    Serial.print("buttonsCheck:\tbuttons: ");
    Serial.println(buttons);
    Serial.print("buttonsCheck:\t(parsed) north / south / east / west / mode:\t");
    Serial.print(north); Serial.print(south); Serial.print(east); Serial.print(west);
    Serial.println(mode);
#endif

    // Handle buttons
    modeButton(mode);
    directionsButtons(north, south, east, west);

    // Set LEDs
    if (MANUAL_MODE) {
        for (uint8_t i = 0; i < 4; i++) {
            tm.setLED(i, (buttons >> i) & 1);
        }
    }
}

void modeButton(uint8_t mode) {
#ifdef DEBUG
    Serial.println("modeButton:\tcalled");
#endif
    if (!mode) {
        // Ignore the button if it's not pressed
        MODE_BUTTON_PRESSED = false;
        return;
    } else if (MODE_BUTTON_PRESSED) {
        // Ignore the button if it's still pressed
        return;
    }

#ifdef DEBUG
    Serial.print("modeButton:\tmode:\t");
    Serial.println(mode);
#endif

    MODE_BUTTON_PRESSED = true;
    MANUAL_MODE = !MANUAL_MODE;

    if (MANUAL_MODE) {
        // If manual mode is enabled disable all tasks except buttonsCheckTask
        executeMovementTask.disable();
        windCheckTask.disable();
        goHomeDarkFeedbackTask.disable();
        displaySensorsTask.disable();

        // Set interval of buttonsCheckTask
        buttonsCheckTask.setInterval(BUTTONS_CHECK_INTERVAL_MANUAL);
        // Init display monitor
        tm.displayText("NSEO----");
    } else {
        // If manual mode is not enabled rehabilitate all tasks and resume routine
        executeMovementTask.enable();
        windCheckTask.enable();
        displaySensorsTask.enable();

        buttonsCheckTask.setInterval(BUTTONS_CHECK_INTERVAL);
        tm.displayText("--------");
    }
}

void directionsButtons(uint8_t north, uint8_t south, uint8_t east, uint8_t west) {
    if (!MANUAL_MODE) {
        return;
    }

    digitalWrite(NORTH_SWITCH, !north);
    digitalWrite(SOUTH_SWITCH, !south);
    digitalWrite(EAST_SWITCH, !east);
    digitalWrite(WEST_SWITCH, !west);
}

void displaySensors() {
    #define CYCLE_SIZE 5

    brightness sensors = readSensors();
    
    String directionStr;
    uint8_t value;

    switch (SENSOR_DISPLAY_CYCLE % CYCLE_SIZE) {
        case 0:
            directionStr = "NORD";
            value = sensors.north;
            break;
        
        case 1:
            directionStr = "SUD ";
            value = sensors.south;
            break;

        case 2:
            directionStr = "EST ";
            value = sensors.east;
            break;

        case 3:
            directionStr = "WEST";
            value = sensors.west;
            break;

        case 4:
            directionStr = "WIND";
            value = readWind();
            break;
    }

    char string[CYCLE_SIZE + 1];
    snprintf(string, CYCLE_SIZE + 1, "%04d", value);
    tm.displayText((directionStr + string).c_str());
    SENSOR_DISPLAY_CYCLE++;
}
