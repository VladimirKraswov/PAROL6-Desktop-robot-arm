/** @file coms_CAN.h
    @brief Заголовочный файл для связи по CAN.
    
    Этот файл предоставляет код для всех прототипов функций файла communication_CAN.cpp.
*/

#ifndef CAN_COMS_H
#define CAN_COMS_H

#include <TMCStepper.h>
#include <Arduino.h>
#include <AccelStepper.h>
#include "hw_init.h"
#include "iodefs.h"
#include "utils.h"
#include "common.h"
#include "constants.h"
#include "adc_init.h"
#include "motor_init.h"
#include "CAN.h"
#include "structs.h"

/* Прототипы функций для работы с шиной CAN */

/// @brief Извлекает ID узла, ID сообщения и бит ошибки из 11-битного CAN ID кадра.
/// @param canId 11-битный CAN ID кадр.
void Extract_from_CAN_ID(unsigned int canId);

/// @brief Объединяет ID_узла, ID_команды и бит_ошибки в стандартный 11-битный CAN ID.
/// @param Node_ID ID узла.
/// @param Command_ID ID команды.
/// @param errorBit бит ошибки.
/// @return стандартный 11-битный CAN ID.
unsigned int Combine_2_CAN_ID(unsigned int Node_ID, unsigned int Command_ID, bool errorBit);

/// @brief Инициализация аппаратной части шины CAN.
void Setup_CAN_bus();

/// @brief Основной протокол обработки входящих и исходящих CAN сообщений.
/// @param Serialport ссылка на объект последовательного порта для отладки.
void CAN_protocol(Stream &Serialport);

/* Команды управления, отправляемые основной платой к периферийным устройствам (например, захвату) */

/// @brief Отправка команды калибровки (захвата).
void Send_gripper_cal();

/// @brief Отправка полного пакета управления (захватом).
void Send_gripper_pack();

/// @brief Отправка команды очистки ошибок (сброса ошибок).
void Send_clear_error();

/// @brief Отправка пустого пакета управления (захватом) для поддержания связи/опроса.
void Send_gripper_pack_empty();

#endif /* CAN_COMS_H */