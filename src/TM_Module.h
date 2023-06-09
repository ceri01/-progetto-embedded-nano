#ifndef _TM_MODULE_H
#define _TM_MODULE_H

/*
    Handle buttons of TM1638plus
*/
void buttonsCheck();

/*
    Dispatch system mode (manual or automatic) when button is pressed
*/
void modeButton(uint8_t);

/*
    Move motor when button is pressed, if manual mode is enabled
*/
void directionsButtons(uint8_t north, uint8_t south, uint8_t east, uint8_t west);

/*
    Display sensor values on TM1638plus's monitor
*/
void displaySensors();

#endif