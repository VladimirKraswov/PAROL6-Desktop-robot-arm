#include "utils.h"

volatile uint32_t counter_1us = 0;  // Счетчик микросекунд

/**
 * Преобразование номера сустава в индекс массива
 */
int JOINT(int joint__) {
    return joint__ - 1;
}

/**
 * Включить питание 24В
 */
void Turn_on_24V(void) {
    digitalWrite(SUPPLY_ON_OFF, HIGH);
}

/**
 * Отключить питание 24В
 */
void Turn_off_24V(void) {
    digitalWrite(SUPPLY_ON_OFF, LOW);
}

/**
 * Управление кнопкой питания (с debounce 3с)
 */
void Power_switch_managment() {
  static uint32_t previous_millis = 0;
  static uint32_t press_count = 0;  // Счетчик для 3с (60 * 50мс = 3с)
  uint32_t current_millis = millis();
  
  if (current_millis - previous_millis >= 50) {
    if (digitalRead(SUPPLY_BUTTON_STATE) == HIGH) {
      press_count++;
      digitalWrite(SUPPLY_ON_OFF, HIGH);
      if (press_count >= 60) {  // 3с
        Turn_off_24V();
        press_count = 0;
      }
    } else {
      press_count = 0;
    }
    previous_millis = current_millis;
  }
}

/**
 * Целое в 3 байта
 */
void intTo3Bytes(int32_t value, byte *bytes) {
    bytes[0] = (value >> 16) & 0xFF;
    bytes[1] = (value >> 8) & 0xFF;
    bytes[2] = value & 0xFF;
}

/**
 * Целое в 2 байта
 */
void intTo2Bytes(int32_t value, byte *bytes) {
    bytes[0] = (value >> 8) & 0xFF;
    bytes[1] = value & 0xFF;
}

/**
 * 3 байта в целое со знаком
 */
int bytes_to_int(uint8_t *bytes) {
    int value = ((int)bytes[0] << 16) | ((int)bytes[1] << 8) | (int)bytes[2];
    if (value & 0x00800000) {
        value |= 0xFF000000;
    }
    return value;
}

/**
 * 2 байта в целое со знаком
 */
int two_bytes_to_int(uint8_t *bytes) {
    int value = ((int)bytes[0] << 8) | (int)bytes[1];
    if (value & 0x00008000) {
        value |= 0xFFFF0000;
    }
    return value;
}

/**
 * Биты в байт
 */
unsigned char bitsToByte(const bool *bits) {
    unsigned char byte = 0;
    for (int i = 0; i < 8; ++i) {
        if (bits[i]) {
            byte |= (1 << (7 - i));
        }
    }
    return byte;
}

/**
 * Байт в биты (little-endian)
 */
void byteToBits(byte b, bool* bits) {
    for (int i = 0; i < 8; i++) {
        bits[i] = (b >> i) & 0x01;
    }
}

/**
 * Байт в биты (big-endian)
 */
void byteToBitsBigEndian(byte b, bool* bits) {
    for (int i = 7; i >= 0; i--) {
        bits[i] = (b >> (7 - i)) & 0x01;
    }
}

// Реализация таймеров как в оригинале
void Ticker_init(TIM_TypeDef *Instance, int frequency, void (*int_callback)()) {
  HardwareTimer *MyTim = new HardwareTimer(Instance);
  MyTim->setOverflow(frequency, HERTZ_FORMAT);
  MyTim->attachInterrupt(int_callback);
  MyTim->resume();
}

void Init_tick_1us() {
  Ticker_init(TIM3, 1000000, tick_1us);
}

void tick_1us() {
    counter_1us++;
}

uint32_t us_tick() {
    return counter_1us;
}