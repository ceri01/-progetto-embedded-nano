#include <TM1638plus.h>

#include "Buttons.h"
#include "Config.h"

extern Task executeMovementTask;
extern Task windCheckTask;
extern Task buttonsCheckTask;

TM1638plus tm(TM_STROBE, TM_CLOCK , TM_DIO, TM_HIGH_FREQ);

bool MANUAL_MODE = false;
bool MODE_BUTTON_PRESSED = false;

void buttonsCheck() {
    const uint8_t buttons = tm.readButtons();
    const uint8_t dir = buttons >> 4;

    // Parse button values
    const uint8_t mode = buttons << 7;
    const uint8_t north = dir >> 3;
    const uint8_t south = (dir >> 2) << 1;
    const uint8_t east = (dir >> 1) << 2;
    const uint8_t west = dir << 3;

    // Handle buttons
    modeButton(mode);
    directionsButtons(north, south, east, west);
}

void modeButton(uint8_t mode) {
    if (!mode) {
        // Ignore the button if it's not pressed
        MODE_BUTTON_PRESSED = false;
        return;
    } else if (MODE_BUTTON_PRESSED) {
        // Ignore the button if it's still pressed
        return;
    }

    MANUAL_MODE = !MANUAL_MODE;

    if (MANUAL_MODE) {
        executeMovementTask.disable();
        windCheckTask.disable();
        buttonsCheckTask.setInterval(BUTTONS_CHECK_INTERVAL_MANUAL);
    } else {
        executeMovementTask.enable();
        windCheckTask.enable();
        buttonsCheckTask.setInterval(BUTTONS_CHECK_INTERVAL);
    }
}

void directionsButtons(uint8_t north, uint8_t south, uint8_t east, uint8_t west) {
    if (!MANUAL_MODE) {
        return;
    }

    digitalWrite(NORTH_SWITCH, north);
    digitalWrite(SOUTH_SWITCH, south);
    digitalWrite(EAST_SWITCH, east);
    digitalWrite(WEST_SWITCH, west);
}