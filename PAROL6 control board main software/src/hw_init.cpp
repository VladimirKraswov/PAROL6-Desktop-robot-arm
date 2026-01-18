#include "hw_init.h"
#include "iodefs.h"
#include <Arduino.h>
#include <SPI.h>

void Init_ADC(void) {
    pinMode(VBUS, INPUT_ANALOG);
}

void Init_USB(void) {
    // Авто в Arduino
}

void Init_Digital_Inputs(void) {
    pinMode(LIMIT1, INPUT);
    pinMode(LIMIT2, INPUT);
    pinMode(LIMIT3, INPUT);
    pinMode(LIMIT4, INPUT);
    pinMode(LIMIT5, INPUT);
    pinMode(LIMIT6, INPUT);
    pinMode(SUPPLY_BUTTON_STATE, INPUT);
    pinMode(INPUT1, INPUT);
    pinMode(INPUT2, INPUT);
    pinMode(ESTOP, INPUT);
}

void Init_Digital_Outputs(void) {
    pinMode(PUL1, OUTPUT);
    pinMode(PUL2, OUTPUT);
    pinMode(PUL3, OUTPUT);
    pinMode(PUL4, OUTPUT);
    pinMode(PUL5, OUTPUT);
    pinMode(PUL6, OUTPUT);

    pinMode(DIR1, OUTPUT);
    pinMode(DIR2, OUTPUT);
    pinMode(DIR3, OUTPUT);
    pinMode(DIR4, OUTPUT);
    pinMode(DIR5, OUTPUT);
    pinMode(DIR6, OUTPUT);

    pinMode(SELECT1, OUTPUT);
    pinMode(SELECT2, OUTPUT);
    pinMode(SELECT3, OUTPUT);
    pinMode(SELECT4, OUTPUT);
    pinMode(SELECT5, OUTPUT);
    pinMode(SELECT6, OUTPUT);
    pinMode(FLASH_SELECT, OUTPUT);

    pinMode(GLOBAL_ENABLE, OUTPUT);
    pinMode(OUTPUT1, OUTPUT);
    pinMode(OUTPUT2, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(SUPPLY_ON_OFF, OUTPUT);
}

void Init_SPI(void) {
    SPI.begin();
}

void Init_CAN(void) {
    // Заглушка (реализуйте если нужно)
}

void Init_ALL_HW(void) {
    Init_Digital_Inputs();
    Init_Digital_Outputs();
    Init_ADC();
    Init_SPI();
}