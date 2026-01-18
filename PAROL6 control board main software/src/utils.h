/** @file utils.h
    @brief Заголовочный файл вспомогательных функций.
    
    Содержит прототипы утилит для работы с роботом.
*/

#ifndef UTILS_
#define UTILS_

#include <Arduino.h>
#include <stdio.h>
#include "iodefs.h"
#include "hw_init.h"
#include <SPI.h>
#include "constants.h"

/* Прототипы функций */

// Преобразование номера сустава (1-6) в индекс массива (0-5)
int JOINT(int joint__);

// Управление питанием 24В
void Turn_on_24V(void);
void Turn_off_24V(void);

// Инициализация таймера с частотой и колбэком
void Ticker_init(TIM_TypeDef *Instance, int frequency, void (*int_callback)());
void Init_tick_1us();
void tick_1us();
uint32_t us_tick();

// Управление кнопкой питания
void Power_switch_managment();

// Преобразования данных
void intTo3Bytes(int32_t value, byte *bytes);
void intTo2Bytes(int32_t value, byte *bytes);
int bytes_to_int(uint8_t *bytes);
int two_bytes_to_int(uint8_t *bytes);
unsigned char bitsToByte(const bool *bits);
void byteToBits(byte b, bool* bits);
void byteToBitsBigEndian(byte b, bool* bits);

#endif /* UTILS_ */