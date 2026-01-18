### **Этап 1: Создание проекта PlatformIO для STM32H723**

Создайте новый проект или модифицируйте существующий `platformio.ini`:

```ini
; PlatformIO Project Configuration File for Mellow Fly 3D Super8
[env:mellow_fly_super8]
platform = ststm32
board = genericSTM32H723ZG  ; Или ближайшая доступная
framework = arduino
upload_protocol = stlink
upload_port = COM8  ; Адаптируйте под ваш порт
debug_tool = stlink

; Важные флаги для STM32H7
build_flags = 
    -D PIO_FRAMEWORK_ARDUINO_ENABLE_CDC
    -D USBCON
    -D USBD_VID=0x0483
    -D USBD_PID=0x5740
    -D USB_MANUFACTURER="Mellow"
    -D ARDUINO_GENERIC_H723ZGTX
    -D HAL_PCD_MODULE_ENABLED
    -D STM32H723xx
    ; Для поддержки CANFD
    -D HAL_FDCAN_MODULE_ENABLED
    ; Для высокой скорости SPI
    -D SPI_SPEED_CLOCK_DIV2_MHZ=50

; Настройки монитора
monitor_port = COM8
monitor_speed = 3000000
monitor_rts = 0
monitor_dtr = 0

; Оптимизация для Cortex-M7
build_unflags = -Os
build_flags += 
    -O2
    -mcpu=cortex-m7
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -mthumb

; Библиотеки (добавятся автоматически через platformio.ini)
lib_deps = 
    https://github.com/teemuatlut/TMCStepper.git
    https://github.com/watterott/AccelStepper.git
    ; Для CAN в Arduino
    https://github.com/sandeepmistry/arduino-CAN.git
```

### **Этап 2: Адаптация iodefs.h для Arduino**

```cpp
// iodefs.h для Mellow Fly 3D Super8 с использованием Arduino pin numbers
#ifndef IODEFS_H
#define IODEFS_H

#include <Arduino.h>

// Определения пинов для Mellow Fly 3D Super8
// НЕОБХОДИМО УТОЧНИТЬ РАСПИНОВКУ!

// Драйверы двигателей (Motor Drivers 1-6)
// Предположительно, пины для шаговых драйверов
#define PUL1    PE_9    // STEP 1
#define DIR1    PE_11   // DIR 1
#define SELECT1 PE_15   // CS 1 (SPI chip select)

#define PUL2    PE_13   // STEP 2
#define DIR2    PE_10   // DIR 2
#define SELECT2 PE_12   // CS 2

#define PUL3    PD_15   // STEP 3
#define DIR3    PD_14   // DIR 3
#define SELECT3 PD_13   // CS 3

#define PUL4    PD_11   // STEP 4
#define DIR4    PD_10   // DIR 4
#define SELECT4 PD_12   // CS 4

#define PUL5    PD_8    // STEP 5
#define DIR5    PD_9    // DIR 5
#define SELECT5 PD_7    // CS 5

#define PUL6    PD_5    // STEP 6
#define DIR6    PD_6    // DIR 6
#define SELECT6 PD_4    // CS 6

// Концевые выключатели (Endstops)
// На плате обычно подписаны как X-, Y-, Z-, Z1-, Z2-, Z3-
#define LIMIT1  PA_0    // X- или Endstop 1
#define LIMIT2  PA_1    // Y- или Endstop 2
#define LIMIT3  PA_2    // Z- или Endstop 3
#define LIMIT4  PA_3    // Z1- или Endstop 4
#define LIMIT5  PA_4    // Z2- или Endstop 5
#define LIMIT6  PA_5    // Z3- или Endstop 6

// CAN - проверьте документацию платы
#define CAN1_TX PB_9    // CAN1_TX
#define CAN1_RX PB_8    // CAN1_RX

// SPI для драйверов TMC5160
#define MOSI    PA_7    // SPI1_MOSI
#define MISO    PA_6    // SPI1_MISO
#define SCK     PA_5    // SPI1_SCK

// Управление питанием
#define SUPPLY_ON_OFF   PC_10   // MOSFET control for 24V
#define SUPPLY_BUTTON_STATE PC_14 // Power button state

// Светодиоды
#define LED1    PB_2    // Статусный LED (обычно зеленый)
#define LED2    PB_10   // Дополнительный LED (обычно красный)

// Цифровые входы/выходы
// На плате обычно есть несколько свободных GPIO
#define INPUT1  PB_6    // Digital Input 1
#define INPUT2  PB_5    // Digital Input 2
#define OUTPUT1 PC_13   // Digital Output 1 (может быть встроенной кнопкой)
#define OUTPUT2 PB_7    // Digital Output 2

// Аварийная остановка
#define ESTOP   PB_14   // E-Stop button

// АЦП для измерения напряжения шины
#define VBUS    PB_0    // ADC1_IN8 (нужно проверить)

// USB (уже определено в Arduino Core)
// #define USB_D_PLUS  PA_12
// #define USB_D_MINUS PA_11

#endif /* IODEFS_H */
```

### **Этап 3: Модификация adc.c/h для Arduino**

```cpp
// adc_init.h - адаптированная версия для Arduino
#ifndef ADC_INIT_H
#define ADC_INIT_H

#include <Arduino.h>

// В Arduino для STM32H7 используется analogRead() 
// Но мы можем использовать HAL через низкоуровневый доступ

#ifdef __cplusplus
extern "C" {
#endif

// Прототипы функций
void MX_ADC1_Init(void);
int ADC_CHANNEL_8_READ_BUS_VOLTAGE(void);
int BUS_voltage(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_INIT_H */
```

```cpp
// adc.cpp - упрощенная версия для Arduino
#include "adc_init.h"
#include "iodefs.h"

// Используем встроенную функцию analogRead
// Но сначала настроим пин для аналогового ввода

void MX_ADC1_Init(void) {
    // В Arduino просто настраиваем пин как аналоговый вход
    pinMode(VBUS, INPUT_ANALOG);
    
    // Для STM32H7 можем настроить разрешение (по умолчанию 12 бит)
    analogReadResolution(12);  // 12-битное разрешение
}

int ADC_CHANNEL_8_READ_BUS_VOLTAGE(void) {
    // Простое чтение АЦП через Arduino API
    return analogRead(VBUS);
}

int BUS_voltage(void) {
    int R1 = 110000; // 110k Ом
    int R2 = 16000;  // 16k Ом
    float referenceVoltage = 3.3; // 3.3В опорное
    int adcResolution = 4095;     // 12-битный АЦП
    
    int adcValue = ADC_CHANNEL_8_READ_BUS_VOLTAGE();
    
    // Коэффициент делителя
    float voltageDividerRatio = (float)R2 / (R1 + R2);
    
    // Рассчет напряжения в милливольтах
    int measuredVoltage = (int)(adcValue * (referenceVoltage * 1000.0f) / adcResolution / voltageDividerRatio);
    
    return measuredVoltage;
}
```

### **Этап 4: Замена CAN библиотеки**

Так как CAN в Arduino для STM32H7 может отличаться, предлагаю использовать библиотеку `arduino-CAN`:

```cpp
// В platformio.ini добавьте:
// lib_deps = sandeepmistry/CAN@^1.2.0

// Модифицированный CAN.h для использования с библиотекой
#ifndef CAN_UTILS_H
#define CAN_UTILS_H

#include <Arduino.h>
#include <CAN.h>  // Библиотека arduino-CAN

#define DEBUG 0

// Типы данных остаются те же
typedef enum {CAN_50KBPS, CAN_100KBPS, CAN_125KBPS, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS} BITRATE;
typedef enum {STANDARD_FORMAT = 0, EXTENDED_FORMAT} CAN_FORMAT;
typedef enum {DATA_FRAME = 0, REMOTE_FRAME} CAN_FRAME;

// Структура CAN сообщения
typedef struct {
  uint32_t id;
  uint8_t  data[8];
  uint8_t  len;
  uint8_t  ch;
  uint8_t  format;
  uint8_t  type;
} CAN_msg_t;

// Конвертация BITRATE в скорость для библиотеки
long bitrateToCanSpeed(BITRATE bitrate) {
  switch(bitrate) {
    case CAN_50KBPS: return 50E3;
    case CAN_100KBPS: return 100E3;
    case CAN_125KBPS: return 125E3;
    case CAN_250KBPS: return 250E3;
    case CAN_500KBPS: return 500E3;
    case CAN_1000KBPS: return 1000E3;
    default: return 125E3;
  }
}

// Инициализация CAN
bool CANInit(BITRATE bitrate, int remap) {
  // Настройка пинов CAN (зависит от платы)
  // Для Mellow Fly: CAN1_TX = PB_9, CAN1_RX = PB_8
  
  // Инициализация библиотеки CAN
  if (!CAN.begin(bitrateToCanSpeed(bitrate))) {
    return false;
  }
  
  // Настройка фильтров (принимать все сообщения)
  CAN.filter(0x0);  // Принимать все сообщения
  
  return true;
}

// Отправка CAN сообщения
void CANSend(uint8_t ch, CAN_msg_t* CAN_tx_msg) {
  CAN.beginPacket(CAN_tx_msg->id, CAN_tx_msg->len);
  CAN.write(CAN_tx_msg->data, CAN_tx_msg->len);
  CAN.endPacket();
}

// Проверка наличия сообщений
uint8_t CANMsgAvail(uint8_t ch) {
  return CAN.available();
}

// Прием CAN сообщения
void CANReceive(uint8_t ch, CAN_msg_t* CAN_rx_msg) {
  if (CAN.parsePacket() > 0) {
    CAN_rx_msg->id = CAN.packetId();
    CAN_rx_msg->len = CAN.packetDlc();
    
    // Чтение данных
    int i = 0;
    while (CAN.available() && i < 8) {
      CAN_rx_msg->data[i++] = CAN.read();
    }
    
    // Заполнение остальных полей
    CAN_rx_msg->format = (CAN.packetExtended() ? EXTENDED_FORMAT : STANDARD_FORMAT);
    CAN_rx_msg->type = DATA_FRAME;  // Библиотека не различает remote frame
  }
}

#endif // CAN_UTILS_H
```

### **Этап 5: Адаптация main.cpp для Arduino**

```cpp
// В начало main.cpp добавьте:
#include <Arduino.h>
#include <SPI.h>
#include <TMCStepper.h>
#include <AccelStepper.h>
#include "hw_init.h"
#include "iodefs.h"
#include "utils.h"
#include "common.h"
#include "constants.h"
#include "adc_init.h"
#include "motor_init.h"
#include "coms_CAN.h"

// Замените HAL_GetTick() на millis()
// Замените delayMicroseconds() на delayMicroseconds() (оно уже есть в Arduino)

// Удалите или закомментируйте:
// #include "stm32f4xx_hal.h"  // Не нужно в Arduino
// TIM_TypeDef *Instance = TIM2;
// HardwareTimer *MyTim = new HardwareTimer(Instance);

// Вместо HardwareTimer используйте встроенные функции Arduino:
unsigned long prev_tick = 0;
unsigned long current_tick = 0;

// В setup():
void setup() {
  // Инициализация Serial (USB)
  Serial.begin(3000000);
  
  // Инициализация пинов
  Init_ALL_HW();
  
  // Инициализация SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4); // Скорость SPI
  
  // Инициализация АЦП
  MX_ADC1_Init();
  
  // Инициализация CAN
  Setup_CAN_bus();
  
  // Инициализация драйверов двигателей
  for (int i = 0; i < 6; i++) {
    Init_motor_drivers(i);
    delay(90);
  }
  
  // Настройка степперов
  for (int i = 0; i < 6; i++) {
    stepper[i].setMaxSpeed(50000);
    stepper[i].setAcceleration(1000);
    stepper[i].setSpeed(0);
  }
  
  // Включение питания
  digitalWrite(SUPPLY_ON_OFF, HIGH);
}

// В loop():
void loop() {
  static uint32_t last_time = 0;
  uint32_t ms = millis();  // Вместо HAL_GetTick()
  
  if ((ms - last_time) > 3000) {
    last_time = ms;
    // Код, выполняемый каждые 3 секунды
  }
  
  // Основная логика управления
  Power_switch_managment();
  
  // Чтение команд и управление двигателями...
  // (остальной код остается похожим, но замените HAL функции на Arduino)
}
```

### **Этап 6: Адаптация утилит (utils.cpp)**

```cpp
// utils.cpp - адаптированный для Arduino
#include "utils.h"
#include <Arduino.h>

// Используем встроенные функции Arduino для времени
void Power_switch_managment() {
  static uint32_t previous_millis = 0;
  static uint32_t turn_off_flag = 0;
  uint32_t current_millis = millis();  // Вместо HAL_GetTick()
  
  if (current_millis - previous_millis >= 50) {
    if (turn_off_flag == 1) {
      turn_off_flag = 0;
      digitalWrite(SUPPLY_ON_OFF, LOW);
    }
    
    if (digitalRead(SUPPLY_BUTTON_STATE) == HIGH) {
      turn_off_flag = 1;
      digitalWrite(SUPPLY_ON_OFF, HIGH);
    }
    
    previous_millis = current_millis;
  }
}

// Функции преобразования данных остаются без изменений
void intTo3Bytes(int32_t value, byte *bytes) {
  bytes[0] = (value >> 16) & 0xFF;
  bytes[1] = (value >> 8) & 0xFF;
  bytes[2] = value & 0xFF;
}

// ... остальные функции остаются такими же
```

### **Этап 7: Проверка и тестирование**

Создайте простой тестовый скетч для проверки пинов:

```cpp
// test_pins.cpp
#include <Arduino.h>
#include "iodefs.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Тестируем выводы
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  // Тестируем входы
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  
  Serial.println("Starting pin test...");
}

void loop() {
  // Мигаем светодиодами
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  delay(500);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  delay(500);
  
  // Читаем входы
  Serial.print("INPUT1: ");
  Serial.print(digitalRead(INPUT1));
  Serial.print(" INPUT2: ");
  Serial.println(digitalRead(INPUT2));
}
```

## 📊 Структура проекта после переноса

```
parol6_mellow_fly/
├── include/
│   ├── iodefs.h          # Адаптированные пины для Mellow Fly
│   ├── constants.h       # Без изменений
│   ├── structs.h         # Без изменений
│   └── adc_init.h        # Упрощенный для Arduino
├── src/
│   ├── main.cpp          # Адаптированный для Arduino
│   ├── adc.cpp           # Упрощенный для Arduino
│   ├── coms_CAN.cpp      # Без изменений (но использует новую CAN библиотеку)
│   ├── motor_init.cpp    # Без изменений
│   ├── hw_init.cpp       # Arduino-версия
│   ├── utils.cpp         # Arduino-версия
│   └── CAN.cpp           # Заменен на использование библиотеки arduino-CAN
├── lib/                  # Библиотеки PlatformIO
├── platformio.ini        # Конфигурация для STM32H723
└── test/
    └── test_pins.cpp     # Тестовый скетч
```

## ⚠️ Ключевые моменты для проверки

1. **Распиновка SPI** - убедитесь, что пины MOSI/MISO/SCK правильные
2. **CAN пины** - проверьте, что PB_8/PB_9 действительно CAN пины
3. **Прерывания** - в Arduino attachInterrupt() может иметь другие номера пинов
4. **Скорость SPI** - TMC5160 требует правильной настройки скорости
5. **Питание 24В** - проверьте логику управления MOSFET

## 🔧 Пошаговый процесс переноса

1. **Начните с тестового проекта** - проверьте светодиоды и кнопки
2. **Добавьте SPI** - проверьте связь с одним TMC5160
3. **Добавьте CAN** - отправьте тестовое сообщение
4. **Интегрируйте одну ось** - проверьте движение
5. **Добавьте остальные оси** 
6. **Протестируйте гоминг**
7. **Интегрируйте связь с ПК**

## 📝 Что нужно уточнить

1. **Точная распиновка Mellow Fly** - найдите схему или pinout диаграмму
2. **Поддерживает ли Arduino Core для H7 все функции** - проверьте форумы
3. **Альтернативные пины** - если предложенные пины не работают
