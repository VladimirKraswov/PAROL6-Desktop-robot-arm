#include "utils.h"

volatile uint32_t counter_1us = 0;

/**
 * Вспомогательная функция для преобразования номера сустава в индекс массива
 */
int JOINT(int joint__) {
    return joint__ - 1;
}

/// @brief Включить питание 24В
void Turn_on_24V(void) {
    digitalWrite(SUPPLY_ON_OFF, HIGH);
}

/// @brief Отключить питание 24В
void Turn_off_24V(void) {
    digitalWrite(SUPPLY_ON_OFF, LOW);
}

/**
 * Управление кнопкой питания
 */
void Power_switch_managment() {
    static uint32_t previous_millis = 0;
    static uint32_t turn_off_flag = 0;
    uint32_t current_millis = millis();
    
    // Проверяем условие каждые 50 мс
    if (current_millis - previous_millis >= 50) {
        // Если был установлен флаг отключения, выполняем отключение
        if (turn_off_flag == 1) {
            turn_off_flag = 0;
            digitalWrite(SUPPLY_ON_OFF, LOW);
        }
        
        // Если кнопка питания нажата
        if (digitalRead(SUPPLY_BUTTON_STATE) == HIGH) {
            turn_off_flag = 1;
            digitalWrite(SUPPLY_ON_OFF, HIGH);
        }
        
        previous_millis = current_millis;
    }
}

/**
 * Преобразует целое число в массив из 3 байт
 */
void intTo3Bytes(int32_t value, byte *bytes) {
    bytes[0] = (value >> 16) & 0xFF;
    bytes[1] = (value >> 8) & 0xFF;
    bytes[2] = value & 0xFF;
}

/**
 * Преобразует целое число в массив из 2 байт
 */
void intTo2Bytes(int32_t value, byte *bytes) {
    bytes[0] = (value >> 8) & 0xFF;
    bytes[1] = value & 0xFF;
}

/**
 * Преобразует массив из 3 байт в целое число со знаком
 */
int bytes_to_int(uint8_t *bytes) {
    int value = ((int)bytes[0] << 16) | ((int)bytes[1] << 8) | (int)bytes[2];
    // Проверка на отрицательное число
    if (value & 0x00800000) {
        value |= 0xFF000000;
    }
    return value;
}

/**
 * Преобразует массив из 2 байт в целое число со знаком
 */
int two_bytes_to_int(uint8_t *bytes) {
    int value = ((int)bytes[0] << 8) | (int)bytes[1];
    // Проверка на отрицательное число
    if (value & 0x00008000) {
        value |= 0xFFFF0000;
    }
    return value;
}

/**
 * Преобразует массив из 8 бит в один байт
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
 * Преобразует байт в массив из 8 бит (little-endian)
 */
void byteToBits(byte b, bool* bits) {
    for (int i = 0; i < 8; i++) {
        bits[i] = (b >> i) & 0x01;
    }
}

/**
 * Преобразует байт в массив из 8 бит (big-endian)
 */
void byteToBitsBigEndian(byte b, bool* bits) {
    for (int i = 7; i >= 0; i--) {
        bits[i] = (b >> (7 - i)) & 0x01;
    }
}