/** @file coms_CAN.cpp
    @brief Реализация протокола CAN.
    
    Этот файл содержит код для реализации протокола CAN.
*/

#include "coms_CAN.h"
#include <Arduino.h>

// Структура CAN ID: 4 + 6 + 1; ПЕРВЫЕ 4 старших бита - это ID узла, следующие 6 - ID команды, последний - бит ошибки.
CAN_msg_t CAN_TX_msg; // Структура для отправляемого CAN сообщения
CAN_msg_t CAN_RX_msg; // Структура для принимаемого CAN сообщения

#define DEBUG_COMS 0 // Флаг отладки для вывода отладочной информации (0 - выключен)

/// @brief Извлекает ID узла, ID сообщения узла и бит ошибки из 11-битного CAN ID кадра
/// @param canId 11-битный CAN ID кадр
void Extract_from_CAN_ID(unsigned int canId)
{
    // Извлечение ID узла (первые 4 старших бита)
    unsigned int ID2 = (canId >> 7) & 0xF;
    // Извлечение CAN команды (следующие 6 бит)
    unsigned int canCommand = (canId >> 1) & 0x3F;
    // Извлечение бита ошибки (последний бит)
    unsigned int errorBit = canId & 0x1;
}

/// @brief Объединяет ID_узла, ID_команды и бит_ошибки в стандартный 11-битный CAN ID
/// @param Node_ID ID узла
/// @param Command_ID ID команды
/// @param errorBit бит ошибки
/// @return стандартный 11-битный CAN ID
unsigned int Combine_2_CAN_ID(unsigned int Node_ID, unsigned int Command_ID, bool errorBit)
{
    // Объединение компонентов в 11-битный CAN ID
    unsigned int canId = 0;
    // Добавление ID узла (первые 4 старших бита)
    canId |= (Node_ID & 0xF) << 7;
    // Добавление CAN команды (следующие 6 бит)
    canId |= (Command_ID & 0x3F) << 1;
    // Добавление бита ошибки (последний бит)
    canId |= (errorBit & 0x1);
    return canId;
}

/// @brief Настройка аппаратной части шины CAN
void Setup_CAN_bus()
{
    bool ret = CANInit(CAN_1000KBPS); // Инициализация CAN с битрейтом 1000 кбит/с
    if (!ret) // Если инициализация не удалась
        while (true) // Зависаем в бесконечном цикле
            ;
}

/// @brief Протокол обработки CAN сообщений
/// @param Serialport ссылка на объект последовательного порта для отладки
void CAN_protocol(Stream &Serialport)
{
    if (CANMsgAvail()) // Проверяем наличие сообщений в CAN
    {
        /// Получить CAN сообщение из буфера
        CANReceive(&CAN_RX_msg);
        /// Распаковать CAN ID
        unsigned int Node_ID = (CAN_RX_msg.id >> 7) & 0xF; // ID узла
        unsigned int Command_ID = (CAN_RX_msg.id >> 1) & 0x3F; // ID команды
        unsigned int Error_bit = CAN_RX_msg.id & 0x1; // Бит ошибки
        /// Вывести данные CAN (только в режиме отладки)
        #if (DEBUG_COMS > 0)
                Serialport.print("Node ID: ");
                Serialport.println(Node_ID);
                Serialport.print("Command ID: ");
                Serialport.println(Command_ID);
                Serialport.print("Error Bit: ");
                Serialport.println(Error_bit);
                Serialport.print("Msg length: ");
                Serialport.println(CAN_RX_msg.len);
                Serialport.println("");
                Serialport.print("Is standard frame: ");
                Serialport.println(CAN_RX_msg.format);
                Serialport.println("");
        #endif
        // Если ID узла соответствует запрашиваемому ID захвата (Comp_gripper.commanded_ID)
        if (Node_ID == Comp_gripper.commanded_ID)
        {
            // Выполнить действие в зависимости от ID команды, полученной ОТ захвата!
            switch (Command_ID)
            {
            case 60: // Пакет данных от захвата (Gripper data pack)
            {
                if (CAN_RX_msg.len == 4) // Проверяем длину пакета (ожидаем 4 байта)
                {
                    /// Структура данных:
                    /// Позиция - 1 байт
                    /// Ток - 2 байта
                    /// 1 байт состояния:
                    /// бит 0 - захват активирован (1) / деактивирован (0)
                    /// бит 1 - статус действия захвата - 1 (перемещение), 0 (бездействие, автоотпускание или калибровка)
                    /// биты 2 и 3 - обнаружение объекта
                    /// бит 4 - ошибка температуры захвата
                    /// бит 5 - ошибка таймаута захвата
                    /// бит 6 - ошибка аварийной остановки (E-Stop) захвата
                    /// бит 7 - статус калибровки захвата; откалиброван (1) / не откалиброван (0)
                    Comp_gripper.current_position = CAN_RX_msg.data[0]; // Текущая позиция
                    uint8_t temp_buffer[] = {CAN_RX_msg.data[1], CAN_RX_msg.data[2]};
                    Comp_gripper.current_current = two_bytes_to_int(temp_buffer); // Текущее значение тока (уставка)
                    //bool bitArray[8];
                    //byteToBitsBigEndian(CAN_RX_msg.data[3], bitArray); // Распаковать биты состояния (альтернативный метод)
                    Comp_gripper.current_status = CAN_RX_msg.data[3]; // Байт состояния
                    #if (DEBUG_COMS > 0) // Отладочный вывод
                    Serialport.print("Position is:");
                    Serialport.println(Comp_gripper.current_position);
                    Serialport.print("Current is:");
                    Serialport.println(Comp_gripper.current_current);
                    Serialport.print("Data byte is: ");
                    Serialport.println(Comp_gripper.current_status);
                    #endif
                    break;
                }
                break;
            }
            // ... здесь могут быть другие case для других команд ...
            }
        }else{
            #if (DEBUG_COMS > 0)
            Serialport.print("Wrong ID"); // Получено сообщение с неправильным ID узла
            #endif
        }
    }
}
/// @brief Отправка команды калибровки захвата; направление: Основная плата -> Захват
void Send_gripper_cal()
{
    // Формирование пустого сообщения калибровки (все данные 0)
    CAN_TX_msg.data[0] = 0x00;
    CAN_TX_msg.data[1] = 0x00;
    CAN_TX_msg.data[2] = 0x00;
    CAN_TX_msg.data[3] = 0x00;
    CAN_TX_msg.data[4] = 0x00;
    CAN_TX_msg.data[5] = 0x00;
    CAN_TX_msg.data[6] = 0x00;
    CAN_TX_msg.data[7] = 0x00;
    CAN_TX_msg.len = 0; // Длина данных 0
    CAN_TX_msg.type = DATA_FRAME; // Тип: кадр данных
    CAN_TX_msg.format = STANDARD_FORMAT; // Формат: стандартный (11 бит)
    // Формирование CAN ID: ID узла + команда 62 (калибровка) + бит ошибки 0
    CAN_TX_msg.id = Combine_2_CAN_ID(Comp_gripper.commanded_ID, 62, 0);
    CANSend(&CAN_TX_msg); // Отправка
}
/// @brief Отправка пакета управления захватом; направление: Основная плата -> Захват
void Send_gripper_pack()
{
    // Структура данных (5 байт):
    // pos setpoint - 1 байт (заданная позиция)
    // speed setpoint - 1 байт (заданная скорость)
    // current setpoint - 2 байта (заданный ток)
    // data - 4 бита: activated, action_status, estop_status, relese_dir (состояние и команды)
    byte data_buffer_send[2];
    intTo2Bytes(Comp_gripper.commanded_current, data_buffer_send); // Преобразование тока в 2 байта
    bool bitArray[8];
    byteToBitsBigEndian(Comp_gripper.command, bitArray); // Распаковка командного байта в биты
    // Заполнение полей сообщения
    CAN_TX_msg.data[0] = Comp_gripper.commanded_position; // Заданная позиция
    CAN_TX_msg.data[1] = Comp_gripper.commanded_speed; // Заданная скорость
    CAN_TX_msg.data[2] = data_buffer_send[0]; // Ток (байт 1)
    CAN_TX_msg.data[3] = data_buffer_send[1]; // Ток (байт 2)
    CAN_TX_msg.data[4] = bitsToByte(bitArray); // Байт состояния/команд
    CAN_TX_msg.data[5] = 0x00; // Неиспользуемые байты
    CAN_TX_msg.data[6] = 0x00;
    CAN_TX_msg.data[7] = 0x00;
    CAN_TX_msg.len = 5; // Длина полезных данных - 5 байт
    CAN_TX_msg.type = DATA_FRAME; // Тип: кадр данных
    CAN_TX_msg.format = STANDARD_FORMAT; // Формат: стандартный (11 бит)
    // Формирование CAN ID: ID узла + команда 61 (управление) + бит ошибки 0
    CAN_TX_msg.id = Combine_2_CAN_ID(Comp_gripper.commanded_ID, 61, 0);
    CANSend(&CAN_TX_msg); // Отправка
}
/// @brief Отправка пустого пакета управления захватом; направление: Основная плата -> Захват
void Send_gripper_pack_empty()
{
    // Формирование пустого сообщения управления (все данные 0)
    CAN_TX_msg.data[0] = 0x00;
    CAN_TX_msg.data[1] = 0x00;
    CAN_TX_msg.data[2] = 0x00;
    CAN_TX_msg.data[3] = 0x00;
    CAN_TX_msg.data[4] = 0x00;
    CAN_TX_msg.data[5] = 0x00;
    CAN_TX_msg.data[6] = 0x00;
    CAN_TX_msg.data[7] = 0x00;
    CAN_TX_msg.len = 0; // Длина данных 0
    CAN_TX_msg.type = DATA_FRAME; // Тип: кадр данных
    CAN_TX_msg.format = STANDARD_FORMAT; // Формат: стандартный (11 бит)
    // Формирование CAN ID: ID узла + команда 61 (управление) + бит ошибки 0
    CAN_TX_msg.id = Combine_2_CAN_ID(Comp_gripper.commanded_ID, 61, 0);
    CANSend(&CAN_TX_msg); // Отправка
}
/// @brief Отправка команды сброса ошибок; направление: Основная плата -> Захват
void Send_clear_error()
{
    // Формирование пустого сообщения сброса ошибок (все данные 0)
    CAN_TX_msg.data[0] = 0x00;
    CAN_TX_msg.data[1] = 0x00;
    CAN_TX_msg.data[2] = 0x00;
    CAN_TX_msg.data[3] = 0x00;
    CAN_TX_msg.data[4] = 0x00;
    CAN_TX_msg.data[5] = 0x00;
    CAN_TX_msg.data[6] = 0x00;
    CAN_TX_msg.data[7] = 0x00;
    CAN_TX_msg.len = 0; // Длина данных 0
    CAN_TX_msg.type = DATA_FRAME; // Тип: кадр данных
    CAN_TX_msg.format = STANDARD_FORMAT; // Формат: стандартный (11 бит)
    // Формирование CAN ID: ID узла + команда 1 (сброс ошибок) + бит ошибки 0
    CAN_TX_msg.id = Combine_2_CAN_ID(Comp_gripper.commanded_ID, 1, 0);
    CANSend(&CAN_TX_msg); // Отправка
}