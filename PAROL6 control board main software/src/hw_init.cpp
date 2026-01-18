/** @file hw_init.cpp
    @brief Исходный файл аппаратной инициализации.
    
    Содержит реализацию функций для настройки аппаратной части.
*/

#include "hw_init.h"
#include "iodefs.h"
#include <Arduino.h>
#include <SPI.h>
#include "structs.h"  // Для доступа к Joint и флагов ошибок
#include "motor_init.h"  // Для инициализации суставов (если нужно знать trigger)

extern MotorStruct Joint[];  // Глобальная структура суставов

void Init_ADC(void) {
    pinMode(VBUS, INPUT_ANALOG);  // adc0 - pf4, -, +
}

void Init_USB(void) {
    // Инициализация USB происходит автоматически в Arduino framework
}

void Init_Digital_Inputs(void) {
    pinMode(LIMIT1, INPUT);  // io0 - pg12, -, +
    pinMode(LIMIT2, INPUT);  // io1 - pg11, -, +
    pinMode(LIMIT3, INPUT);  // io2 - pg10, -, +
    pinMode(LIMIT4, INPUT);  // io3 - pg9, -, +
    pinMode(LIMIT5, INPUT);  // io4 - pd7, -, +
    pinMode(LIMIT6, INPUT);  // io5 - pd6, -, +
    pinMode(SUPPLY_BUTTON_STATE, INPUT);  // exp1 - pc14
    pinMode(INPUT1, INPUT);  // exp2 - pb6
    pinMode(INPUT2, INPUT);  // exp2 - pb7
    pinMode(ESTOP, INPUT);  // io2 - pg10 (remap, если конфликт)
}

void Init_Digital_Outputs(void) {
    pinMode(PUL1, OUTPUT);  // step driver 0 - step(pe2)
    pinMode(PUL2, OUTPUT);  // step driver 1 - step(pe3)
    pinMode(PUL3, OUTPUT);  // step driver 2 - step(pe4)
    pinMode(PUL4, OUTPUT);  // step driver 3 - step(pe14)
    pinMode(PUL5, OUTPUT);  // step driver 4 - step(pe15)
    pinMode(PUL6, OUTPUT);  // step driver 5 - step(pe1)

    pinMode(DIR1, OUTPUT);  // step driver 0 - dir (pc5)
    pinMode(DIR2, OUTPUT);  // step driver 1 - dir (pf13)
    pinMode(DIR3, OUTPUT);  // step driver 2 - dir (pg0)
    pinMode(DIR4, OUTPUT);  // step driver 3 - dir (pe8)
    pinMode(DIR5, OUTPUT);  // step driver 4 - dir (pe11)
    pinMode(DIR6, OUTPUT);  // step driver 5 - dir (pf0)

    pinMode(SELECT1, OUTPUT);  // step driver 0 - cs/uart (pc4)
    pinMode(SELECT2, OUTPUT);  // step driver 1 - cs/uart (pf12)
    pinMode(SELECT3, OUTPUT);  // step driver 2 - cs/uart (pf15)
    pinMode(SELECT4, OUTPUT);  // step driver 3 - cs/uart (pe7)
    pinMode(SELECT5, OUTPUT);  // step driver 4 - cs/uart (pe10)
    pinMode(SELECT6, OUTPUT);  // step driver 5 - cs/uart (pf1)
    pinMode(FLASH_SELECT, OUTPUT);  // exp2 - pa4

    pinMode(GLOBAL_ENABLE, OUTPUT);  // io6 - pa8, -, +
    pinMode(OUTPUT1, OUTPUT);  // io0 - pg12 (remap если конфликт с LIMIT1)
    pinMode(OUTPUT2, OUTPUT);  // io1 - pg11
    pinMode(LED1, OUTPUT);  // exp1 - pb2
    pinMode(LED2, OUTPUT);  // fan6 - pb10 (remap если фанат нужен)
    pinMode(SUPPLY_ON_OFF, OUTPUT);  // exp1 - pc13
}

void Init_SPI(void) {
    SPI.begin();
}

void Init_CAN(void) {
    // Заглушка: реализуйте полную инициализацию CAN, если требуется (используйте FDCAN для STM32H7)
}

void Init_False_Trigger_ISR(void) {
    // Настройка прерываний на основе limit_switch_trigger из структур суставов
    // Примечание: Суставы должны быть инициализированы перед вызовом этой функции
    attachInterrupt(digitalPinToInterrupt(Joint[5].LIMIT), False_Swtich_Trigger_6, (Joint[5].limit_switch_trigger == 0 ? FALLING : RISING));  // J6 на io5 - pd6, -, +
    attachInterrupt(digitalPinToInterrupt(Joint[1].LIMIT), False_Swtich_Trigger_2, (Joint[1].limit_switch_trigger == 0 ? FALLING : RISING));  // J2 на io1 - pg11, -, +
    attachInterrupt(digitalPinToInterrupt(Joint[2].LIMIT), False_Swtich_Trigger_3, (Joint[2].limit_switch_trigger == 0 ? FALLING : RISING));  // J3 на io2 - pg10, -, +
    attachInterrupt(digitalPinToInterrupt(Joint[3].LIMIT), False_Swtich_Trigger_4, (Joint[3].limit_switch_trigger == 0 ? FALLING : RISING));  // J4 на io3 - pg9, -, +
    attachInterrupt(digitalPinToInterrupt(Joint[4].LIMIT), False_Swtich_Trigger_5, (Joint[4].limit_switch_trigger == 0 ? FALLING : RISING));  // J5 на io4 - pd7, -, +
    attachInterrupt(digitalPinToInterrupt(Joint[0].LIMIT), False_Swtich_Trigger_1, (Joint[0].limit_switch_trigger == 0 ? FALLING : RISING));  // J1 на io0 - pg12, -, +
}

void Enable_False_Trigger_ISR(void) {
    // Функция для повторного включения прерываний (если нужно)
    Init_False_Trigger_ISR();
}

void Disable_False_Trigger_ISR(void) {
    detachInterrupt(digitalPinToInterrupt(Joint[5].LIMIT));
    detachInterrupt(digitalPinToInterrupt(Joint[1].LIMIT));
    detachInterrupt(digitalPinToInterrupt(Joint[2].LIMIT));
    detachInterrupt(digitalPinToInterrupt(Joint[3].LIMIT));
    detachInterrupt(digitalPinToInterrupt(Joint[4].LIMIT));
    detachInterrupt(digitalPinToInterrupt(Joint[0].LIMIT));
}

void False_Swtich_Trigger_1(void) {
    // Обработка ложного срабатывания для сустава 1 (J1)
    Joint[0].error = 1;  // Установка флага общей ошибки
}

void False_Swtich_Trigger_2(void) {
    Joint[1].error = 1;
}

void False_Swtich_Trigger_3(void) {
    Joint[2].error = 1;
}

void False_Swtich_Trigger_4(void) {
    Joint[3].error = 1;
}

void False_Swtich_Trigger_5(void) {
    Joint[4].error = 1;
}

void False_Swtich_Trigger_6(void) {
    Joint[5].error = 1;
}

void Init_ALL_HW(void) {
    Init_Digital_Inputs();
    Init_Digital_Outputs();
    Init_ADC();
    Init_SPI();
    Init_CAN();
    Init_False_Trigger_ISR();  // Добавлено: инициализация прерываний
}