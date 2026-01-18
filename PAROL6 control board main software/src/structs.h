/** @file structs.h
    @brief Заголовочный файл структур данных.
    
    Содержит структуры для моторов, робота и гриппера.
*/

#ifndef STRUCTS
#define STRUCTS

#include <Arduino.h>

/**
 * @brief Структура для одного сустава/мотора.
 */
struct MotorStruct {
    int motor_max_current = 100;  // Максимальный ток (RMS)
    int homing_speed = 100;  // Скорость гоминга
    int motor_max_speed = 0;  // Макс. скорость
    int motor_max_acceleration = 0;  // Макс. ускорение
    int motor_min_speed = 0;  // Мин. скорость
    int motor_min_acceleration = 0;  // Мин. ускорение
    float joint_range_positive;  // Положительный диапазон (радианы)
    float joint_range_negative;  // Отрицательный диапазон (радианы)
    float joint_range_positive_steps;  // Положительный диапазон (шаги)
    float joint_range_negative_steps;  // Отрицательный диапазон (шаги)
    int standby_position;  // Позиция ожидания
    float homed_position;  // Позиция после гоминга
    int homed;  // Флаг гоминга завершено
    int homing;  // Флаг гоминг в процессе
    int homing_stage_1 = 0;  // Этап 1 гоминга
    int homing_stage_2 = 0;  // Этап 2 гоминга
    int temperature_error;  // Ошибка температуры
    int temperature_warrning;  // Предупреждение температуры
    int microstep;  // Микрошаг
    int limit_switch_trigger;  // Триггер концевика
    float reduction_ratio;  // Передаточное отношение
    long position;  // Текущая позиция
    int speed;  // Текущая скорость
    int current;  // Текущий ток
    int LIMIT;  // Пин концевика
    int DIR;  // Пин направления
    int STEP;  // Пин шага
    int SELECT;  // Пин CS SPI
    int driver_chip_on_board;  // Драйвер на плате
    int direction_reversed;  // Обратное направление
    int ihold;  // Ток удержания
    int irun;  // Ток работы
    float hold_multiplier;  // Множитель удержания
    int over_temp_pre_warning;  // Предупреждение перегрева
    int over_temp_warning;  // Ошибка перегрева
    int diag0;  // Диагностика 0
    int open_load_B;  // Обрыв B
    int open_load_A;  // Обрыв A
    int short_2_gnd_B;  // КЗ B
    int short_2_gnd_A;  // КЗ A
    int commaned_mode;  // Режим команды
    int commanded_position;  // Командная позиция
    int commanded_velocity;  // Командная скорость
    int commanded_current;  // Командный ток
    int position_error = 0;  // Ошибка позиции
    int error;  // Общая ошибка
};

/**
 * @brief Структура для робота в целом.
 */
struct Robot {
    int CAN_ID;  // ID CAN
    int CRC_value = 212;  // CRC
    int command;  // Текущая команда
    int affected_joint;  // Сустав для команды
    int commanded_OUT1;  // Команда OUT1
    int commanded_OUT2;  // Команда OUT2
    int In1;  // Вход 1
    int In2;  // Вход 2
    int Out1;  // Выход 1
    int Out2;  // Выход 2
    int Estop;  // E-Stop
    unsigned int time_between_commands = 0;  // Время между командами
    int Out1_commanded;  // Команда OUT1
    int Out2_commanded;  // Команда OUT2
    int xtr2_byte = 8;  // Доп. байт
    int Timeout = 0;  // Таймаут
    int timeout_error = 100;  // Ошибка таймаута
    int Affected_joint;  // Сустав (дубликат)
    int disabled = 0;  // Флаг отключения
};

/**
 * @brief Структура для гриппера.
 */
struct Gripper {
    int current_position = 0;  // Текущая позиция
    int current_speed = 1;  // Текущая скорость
    int current_current = 2;  // Текущий ток
    int current_status = 1;  // Статус
    int object_detection = 1;  // Обнаружение объекта
    int Gripper_ID = 0;  // ID гриппера
    int commanded_position = 0;  // Командная позиция
    int commanded_speed = 0;  // Командная скорость
    int commanded_current = 0;  // Командный ток
    int command = 3;  // Команда
    int mode;  // Режим
    int commanded_ID;  // Командный ID
    int prev_commanded_position;  // Предыдущая позиция
    int prev_commanded_speed;  // Предыдущая скорость
    int prev_commanded_current;  // Предыдущий ток
    int prev_command;  // Предыдущая команда
    int prev_mode;  // Предыдущий режим
    int prev_commanded_ID;  // Предыдущий ID
};

extern Gripper Comp_gripper;  // Глобальный гриппер

#endif /* STRUCTS */