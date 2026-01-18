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
    pinMode(VBUS, INPUT_ANALOG);
}

void Init_USB(void) {
    // Инициализация USB происходит автоматически в Arduino framework
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
    // Заглушка: реализуйте полную инициализацию CAN, если требуется (используйте FDCAN для STM32H7)
}

void Init_False_Trigger_ISR(void) {
    // Настройка прерываний на основе limit_switch_trigger из структур суставов
    // Примечание: Суставы должны быть инициализированы перед вызовом этой функции
    attachInterrupt(digitalPinToInterrupt(Joint[5].LIMIT), False_Swtich_Trigger_6, (Joint[5].limit_switch_trigger == 0 ? FALLING : RISING));  // J6 на LIMIT1
    attachInterrupt(digitalPinToInterrupt(Joint[1].LIMIT), False_Swtich_Trigger_2, (Joint[1].limit_switch_trigger == 0 ? FALLING : RISING));  // J2 на LIMIT2
    attachInterrupt(digitalPinToInterrupt(Joint[2].LIMIT), False_Swtich_Trigger_3, (Joint[2].limit_switch_trigger == 0 ? FALLING : RISING));  // J3 на LIMIT3
    attachInterrupt(digitalPinToInterrupt(Joint[3].LIMIT), False_Swtich_Trigger_4, (Joint[3].limit_switch_trigger == 0 ? FALLING : RISING));  // J4 на LIMIT4
    attachInterrupt(digitalPinToInterrupt(Joint[4].LIMIT), False_Swtich_Trigger_5, (Joint[4].limit_switch_trigger == 0 ? FALLING : RISING));  // J5 на LIMIT5
    attachInterrupt(digitalPinToInterrupt(Joint[0].LIMIT), False_Swtich_Trigger_1, (Joint[0].limit_switch_trigger == 0 ? FALLING : RISING));  // J1 на LIMIT6
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