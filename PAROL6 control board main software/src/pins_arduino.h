// src/pins_arduino.h - переопределения для Mellow Fly Super8
#ifndef PINS_ARDUINO_MF_H
#define PINS_ARDUINO_MF_H

#include <Arduino.h>

// Переопределяем проблемные пины
#undef PC_3_C
#define PC_3_C PC3

#undef PC_2_C  
#define PC_2_C PC2

// Определения пинов Mellow Fly Super8
#define VBUS_PIN PB0
#define LED1_PIN PB2
#define LED2_PIN PB10

// SPI пины
#define MISO_PIN PA6
#define MOSI_PIN PA7
#define SCK_PIN PA5
#define FLASH_SELECT_PIN PA4

// Концевые выключатели
#define LIMIT1_PIN PA0
#define LIMIT2_PIN PA1
#define LIMIT3_PIN PA2
#define LIMIT4_PIN PA3
#define LIMIT5_PIN PA4
#define LIMIT6_PIN PA5

#endif