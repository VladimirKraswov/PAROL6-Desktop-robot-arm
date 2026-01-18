#ifndef IODEFS_H
#define IODEFS_H

#include <Arduino.h>

// ============================================================
// Шаговые драйверы (Step/Direction/CS для 6 осей робота)
// ============================================================

// Ось 1 (соответствует step driver 0)
#define PUL1    PE2    // STEP (step driver 0 - step(pe2))
#define DIR1    PC5    // DIR (step driver 0 - dir (pc5))
#define SELECT1 PC4    // CS (step driver 0 - cs/uart (pc4))

// Ось 2 (step driver 1)
#define PUL2    PE3    // STEP (step driver 1 - step(pe3))
#define DIR2    PF13   // DIR (step driver 1 - dir (pf13))
#define SELECT2 PF12   // CS (step driver 1 - cs/uart (pf12))

// Ось 3 (step driver 2)
#define PUL3    PE4    // STEP (step driver 2 - step(pe4))
#define DIR3    PG0    // DIR (step driver 2 - dir (pg0))
#define SELECT3 PF15   // CS (step driver 2 - cs/uart (pf15))

// Ось 4 (step driver 3)
#define PUL4    PE14   // STEP (step driver 3 - step(pe14))
#define DIR4    PE8    // DIR (step driver 3 - dir (pe8))
#define SELECT4 PE7    // CS (step driver 3 - cs/uart (pe7))

// Ось 5 (step driver 4)
#define PUL5    PE15   // STEP (step driver 4 - step(pe15))
#define DIR5    PE11   // DIR (step driver 4 - dir (pe11))
#define SELECT5 PE10   // CS (step driver 4 - cs/uart (pe10))

// Ось 6 (step driver 5)
#define PUL6    PE1    // STEP (step driver 5 - step(pe1))
#define DIR6    PF0    // DIR (step driver 5 - dir (pf0))
#define SELECT6 PF1    // CS (step driver 5 - cs/uart (pf1))

// ============================================================
// Концевые выключатели (Endstops) — remap на fan, так как совпадают
// ============================================================
#define LIMIT1  PA0    // Endstop 1 (fan0 - pa0)
#define LIMIT2  PA1    // Endstop 2 (fan1 - pa1)
#define LIMIT3  PA2    // Endstop 3 (fan2 - pa2)
#define LIMIT4  PA3    // Endstop 4 (fan3 - pa3)
#define LIMIT5  PA15   // Endstop 5 (fan4 - pa15)
#define LIMIT6  PB11   // Endstop 6 (fan5 - pb11, если fan свободен; иначе io0 - pg12)

// ============================================================
// Управление питанием — remap на exp1
// ============================================================
#define GLOBAL_ENABLE   PA3    // Аппаратное включение/выключение драйверов (fan3 - pa3, если свободен)
#define SUPPLY_ON_OFF   PC13   // MOSFET управление 24V питанием (exp1 - pc13)
#define SUPPLY_BUTTON_STATE PC14 // Состояние кнопки питания (exp1 - pc14)

// ============================================================
// SPI для драйверов TMC5160 — совпадает
// ============================================================
#define MISO    PB4    // SPI1_MISO (TMC MISO - pb4)
#define MOSI    PB5    // SPI1_MOSI (TMC MOSI - pb5)
#define SCK     PB3    // SPI1_SCK (TMC sck - pb3)
#define FLASH_SELECT PA4  // CS для Flash памяти (exp2 - pa4, если используется)

// ============================================================
// Светодиоды индикации — remap на exp1/fan
// ============================================================
#define LED1    PB2    // Статусный светодиод 1 (exp1 - pb2)
#define LED2    PB10   // Статусный светодиод 2 (fan6 - pb10)

// ============================================================
// Цифровые входы/выходы общего назначения — remap на io/exp2
// ============================================================
#define INPUT1  PB6    // Цифровой вход 1 (exp2 - pb6)
#define INPUT2  PB7    // Цифровой вход 2 (exp2 - pb7)
#define OUTPUT1 PG12   // Цифровой выход 1 (io0 - pg12)
#define OUTPUT2 PG11   // Цифровой выход 2 (io1 - pg11)

// ============================================================
// Аварийная остановка — remap на io
// ============================================================
#define ESTOP   PG10   // Кнопка E-Stop (io2 - pg10)

// ============================================================
// АЦП для измерения напряжения шины — remap на adc0
// ============================================================
#define VBUS    PF4    // ADC1_IN для измерения (adc0 - pf4, -, +)

// ============================================================
// CAN интерфейс — совпадает
// ============================================================
#define CAN1_TX PB9    // CAN1_TX (can - canh (pb9))
#define CAN1_RX PB8    // CAN1_RX (can - canl (pb8))

// Другие: Bed (pe5 - bed +, pe5), Screen (pa10-pa9 - screen pa10, pa9)

#endif /* IODEFS_H */