/** @file hw_init.h (заголовочный файл hw_init.h)
    @brief Документированный файл.
    
    Заголовочный файл аппаратной инициализации.
    Для схемы обратитесь к:
    Для дополнительной функциональности выводов обратитесь к:

*/

#ifndef HW_SETUP_H
#define HW_SETUP_H

#include <Arduino.h>
#include <stdio.h>

/* Прототипы функций для настройки аппаратной части */

/// Инициализировать все пины АЦП (Initialize all ADC pins)
void Init_ADC (void);
/// Инициализировать USB порт (Initialize USB port)
void Init_USB(void);
/// Инициализировать пины цифровых входов (Initialize digital input pins)
void Init_Digital_Inputs(void);
/// Инициализировать пины цифровых выходов (Initialize digital output pins)
void Init_Digital_Outputs(void);
/// Инициализировать SPI (Initialize SPI)
void Init_SPI(void);
/// Инициализировать CAN (Initialize CAN)
void Init_CAN(void);
/// Инициализировать всю аппаратную часть (Initialize all hardware)
void Init_ALL_HW(void);
/// Инициализировать ISR для ложных срабатываний (Initialize false trigger ISR)
void Init_False_Trigger_ISR();
/// Включить ISR для ложных срабатываний (Enable false trigger ISR)
void Enable_False_Trigger_ISR();
/// Отключить ISR для ложных срабатываний (Disable false trigger ISR)
void Disable_False_Trigger_ISR();

/* Обработчики прерываний (ISR) для концевых выключателей */

/// @brief Обработчик прерывания для концевого выключателя 1
void False_Swtich_Trigger_1(void);
/// @brief Обработчик прерывания для концевого выключателя 2
void False_Swtich_Trigger_2(void);
/// @brief Обработчик прерывания для концевого выключателя 3
void False_Swtich_Trigger_3(void);
/// @brief Обработчик прерывания для концевого выключателя 4
void False_Swtich_Trigger_4(void);
/// @brief Обработчик прерывания для концевого выключателя 5
void False_Swtich_Trigger_5(void);
/// @brief Обработчик прерывания для концевого выключателя 6
void False_Swtich_Trigger_6(void);

#endif /* HW_SETUP_H */