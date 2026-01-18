/// ЗДЕСЬ ИНИЦИАЛИЗИРУЕМ СТРУКТУРЫ?? (HERE INIT STRUCTURES??)
#include "motor_init.h"

void Init_motor_struct(struct MotorStruct *Joint__)
{
    Joint__->LIMIT = 200; // Пример начального значения (Example initial value)
}

/**
 * Инициализация параметров для сустава 1 (оси 1).
 * Примечание: J1 является особым, потому что мы обычно переходим в позицию,
 * которая находится на +90 градусов от его позиции ожидания по Денавиту-Хартенбергу.
 * +90 градусов эквивалентно 9600 шагам в режиме 32 микрошага.
 */
void Init_Joint_1(struct MotorStruct *Joint__)
{
    Joint__->standby_position = 10240;          // Позиция ожидания (standby position) (10240, ранее 9600)
    Joint__->homed_position = 13500;            // Позиция после успешного поиска нуля (гоминга) (homed position)
    Joint__->motor_max_current = MOTOR1_MAX_CURRENT; // Максимальный ток двигателя (из constants.h)
    Joint__->motor_max_speed = 0;
    Joint__->motor_max_acceleration = 0;
    Joint__->motor_min_speed = 0;
    Joint__->motor_min_acceleration = 0;
    Joint__->joint_range_positive = 0;          // Положительный диапазон движения (в градусах?)
    Joint__->joint_range_negative = 0;          // Отрицательный диапазон движения (в градусах?)
    Joint__->joint_range_positive_steps = 14000;  // Положительный диапазон движения (в шагах)
    Joint__->joint_range_negative_steps = -14000; // Отрицательный диапазон движения (в шагах)
    Joint__->homed = 0;                         // Флаг: завершен ли гоминг для этого сустава? (0 - нет)
    Joint__->homing = 0;                        // Флаг: выполняется ли сейчас гоминг?
    Joint__->temperature_error = 0;             // Флаг ошибки температуры
    Joint__->temperature_warrning = 0;          // Флаг предупреждения о температуре
    Joint__->microstep = MICROSTEP;             // Коэффициент микрошага (из constants.h, обычно 32)
    Joint__->limit_switch_trigger = 0;          // Условие срабатывания концевого выключателя (0 = низкий уровень? Временное значение из-за неисправного датчика?) (temp since sensor is faulty?)
    Joint__->reduction_ratio = 6.4;             // Передаточное отношение редуктора (96 / 15)
    Joint__->position = -200;                   // Текущая позиция (начальное значение)
    Joint__->speed = -100;                      // Текущая скорость (начальное значение)
    Joint__->current = 0;                       // Текущий ток
    Joint__->LIMIT = LIMIT6;                    // Пин подключения концевого выключателя (io5 - pd6, -, +)
    Joint__->DIR = DIR1;                        // Пин направления вращения двигателя (step driver 0 - dir (pc5))
    Joint__->STEP = PUL1;                       // Пин шага (импульса) двигателя (step driver 0 - step(pe2))
    Joint__->SELECT = SELECT1;                  // Пин выбора (CS) драйвера двигателя на шине SPI (step driver 0 - cs/uart (pc4))
    Joint__->driver_chip_on_board = 0;          // Флаг: драйвер на плате? (0 - нет, используется внешний модуль?)
    Joint__->direction_reversed = 1;            // Обратить направление вращения? (1 - да)
    Joint__->irun = 16;                         // Параметр драйвера: ток при движении (I_RUN)
    Joint__->ihold = 16;                        // Параметр драйвера: ток удержания (I_HOLD)
    Joint__->hold_multiplier = 0.7;             // Множитель тока удержания (относительно I_RUN)
    Joint__->over_temp_pre_warning = 0;         // Флаг предупреждения о перегреве драйвера
    Joint__->over_temp_warning = 0;             // Флаг ошибки перегрева драйвера
    Joint__->diag0 = 0;                         // Состояние диагностического вывода DIAG0 драйвера
    Joint__->open_load_B = 0;                   // Обрыв обмотки B (Open Load B)
    Joint__->open_load_A = 0;                   // Обрыв обмотки A (Open Load A)
    Joint__->short_2_gnd_B = 0;                 // Замыкание на землю обмотки B (Short to Gnd B)
    Joint__->short_2_gnd_A = 0;                 // Замыкание на землю обмотки A (Short to Gnd A)
    Joint__->position_error = 0;                // Флаг ошибки позиционирования
}

/**
 * Инициализация параметров для сустава 2 (оси 2).
 */
void Init_Joint_2(struct MotorStruct *Joint__)
{
    Joint__->standby_position = -32000;
    Joint__->homed_position = 19588;
    Joint__->motor_max_current = MOTOR2_MAX_CURRENT;
    Joint__->motor_max_speed = 0;
    Joint__->motor_max_acceleration = 0;
    Joint__->motor_min_speed = 0;
    Joint__->motor_min_acceleration = 0;
    Joint__->joint_range_positive = 0;
    Joint__->joint_range_negative = 0;
    Joint__->joint_range_positive_steps = -1200;
    Joint__->joint_range_negative_steps = -51587;
    Joint__->homed = 0;
    Joint__->homing = 0;
    Joint__->temperature_error = 0;
    Joint__->temperature_warrning = 0;
    Joint__->microstep = MICROSTEP;
    Joint__->limit_switch_trigger = 1;          // Условие срабатывания: 1 (высокий уровень?)
    Joint__->reduction_ratio = 20;              // Передаточное отношение редуктора
    Joint__->position = 0;
    Joint__->speed = 0;
    Joint__->current = 0;
    Joint__->LIMIT = LIMIT2;                    // io1 - pg11, -, +
    Joint__->DIR = DIR2;                        // step driver 1 - dir (pf13)
    Joint__->STEP = PUL2;                       // step driver 1 - step(pe3)
    Joint__->SELECT = SELECT2;                  // step driver 1 - cs/uart (pf12)
    Joint__->driver_chip_on_board = 0;
    Joint__->direction_reversed = 0;            // Направление не обращено
    Joint__->irun = 16;
    Joint__->ihold = 16;
    Joint__->hold_multiplier = 0.8;
    Joint__->over_temp_pre_warning = 0;
    Joint__->over_temp_warning = 0;
    Joint__->diag0 = 0;
    Joint__->open_load_B = 0;
    Joint__->open_load_A = 0;
    Joint__->short_2_gnd_B = 0;
    Joint__->short_2_gnd_A = 0;
    Joint__->position_error = 0;
}

/**
 * Инициализация параметров для сустава 3 (оси 3).
 */
void Init_Joint_3(struct MotorStruct *Joint__)
{
    Joint__->standby_position = 57905;
    Joint__->homed_position = 23020;
    Joint__->motor_max_current = MOTOR3_MAX_CURRENT;
    Joint__->motor_max_speed = 0;
    Joint__->motor_max_acceleration = 0;
    Joint__->motor_min_speed = 0;
    Joint__->motor_min_acceleration = 0;
    Joint__->joint_range_positive = 0;
    Joint__->joint_range_negative = 0;
    Joint__->joint_range_positive_steps = 92605;
    Joint__->joint_range_negative_steps = 34700;
    Joint__->homed = 0;
    Joint__->homing = 0;
    Joint__->temperature_error = 0;
    Joint__->temperature_warrning = 0;
    Joint__->microstep = MICROSTEP;
    Joint__->limit_switch_trigger = 1;
    Joint__->reduction_ratio = 18.0952381;      // 20 * (38 / 42)
    Joint__->position = 0;
    Joint__->speed = 0;
    Joint__->current = 0;
    Joint__->LIMIT = LIMIT3;                    // io2 - pg10, -, +
    Joint__->DIR = DIR3;                        // step driver 2 - dir (pg0)
    Joint__->STEP = PUL3;                       // step driver 2 - step(pe4)
    Joint__->SELECT = SELECT3;                  // step driver 2 - cs/uart (pf15)
    Joint__->driver_chip_on_board = 0;
    Joint__->direction_reversed = 1;
    Joint__->irun = 16;
    Joint__->ihold = 16;
    Joint__->hold_multiplier = 0.8;
    Joint__->over_temp_pre_warning = 0;
    Joint__->over_temp_warning = 0;
    Joint__->diag0 = 0;
    Joint__->open_load_B = 0;
    Joint__->open_load_A = 0;
    Joint__->short_2_gnd_B = 0;
    Joint__->short_2_gnd_A = 0;
    Joint__->position_error = 0;
}

/**
 * Инициализация параметров для сустава 4 (оси 4).
 */
void Init_Joint_4(struct MotorStruct *Joint__)
{
    Joint__->standby_position = 0;
    Joint__->homed_position = -10200;
    Joint__->motor_max_current = MOTOR4_MAX_CURRENT;
    Joint__->motor_max_speed = 0;
    Joint__->motor_max_acceleration = 0;
    Joint__->motor_min_speed = 0;
    Joint__->motor_min_acceleration = 0;
    Joint__->joint_range_positive = 0;
    Joint__->joint_range_negative = 0;
    Joint__->joint_range_positive_steps = 7500;
    Joint__->joint_range_negative_steps = -7500;
    Joint__->homed = 0;
    Joint__->homing = 0;
    Joint__->temperature_error = 0;
    Joint__->temperature_warrning = 0;
    Joint__->microstep = MICROSTEP;
    Joint__->limit_switch_trigger = 0;
    Joint__->reduction_ratio = 4;
    Joint__->position = 0;
    Joint__->speed = 0;
    Joint__->current = 0;
    Joint__->LIMIT = LIMIT4;                    // io3 - pg9, -, +
    Joint__->DIR = DIR4;                        // step driver 3 - dir (pe8)
    Joint__->STEP = PUL4;                       // step driver 3 - step(pe14)
    Joint__->SELECT = SELECT4;                  // step driver 3 - cs/uart (pe7)
    Joint__->driver_chip_on_board = 0;
    Joint__->direction_reversed = 0;
    Joint__->irun = 16;
    Joint__->ihold = 16;
    Joint__->hold_multiplier = 0.8;
    Joint__->over_temp_pre_warning = 0;
    Joint__->over_temp_warning = 0;
    Joint__->diag0 = 0;
    Joint__->open_load_B = 0;
    Joint__->open_load_A = 0;
    Joint__->short_2_gnd_B = 0;
    Joint__->short_2_gnd_A = 0;
    Joint__->position_error = 0;
}

/**
 * Инициализация параметров для сустава 5 (оси 5).
 */
void Init_Joint_5(struct MotorStruct *Joint__)
{
    Joint__->standby_position = 0;
    Joint__->homed_position = 8900;
    Joint__->motor_max_current = MOTOR5_MAX_CURRENT;
    Joint__->motor_max_speed = 0;
    Joint__->motor_max_acceleration = 0;
    Joint__->motor_min_speed = 0;
    Joint__->motor_min_acceleration = 0;
    Joint__->joint_range_positive = 0;
    Joint__->joint_range_negative = 0;
    Joint__->joint_range_positive_steps = 6400;
    Joint__->joint_range_negative_steps = -6400;
    Joint__->homed = 0;
    Joint__->homing = 0;
    Joint__->temperature_error = 0;
    Joint__->temperature_warrning = 0;
    Joint__->microstep = MICROSTEP;
    Joint__->limit_switch_trigger = 1;
    Joint__->reduction_ratio = 4;
    Joint__->position = 0;
    Joint__->speed = 0;
    Joint__->current = 0;
    Joint__->LIMIT = LIMIT5;                    // io4 - pd7, -, +
    Joint__->DIR = DIR5;                        // step driver 4 - dir (pe11)
    Joint__->STEP = PUL5;                       // step driver 4 - step(pe15)
    Joint__->SELECT = SELECT5;                  // step driver 4 - cs/uart (pe10)
    Joint__->driver_chip_on_board = 0;
    Joint__->direction_reversed = 0;
    Joint__->irun = 16;
    Joint__->ihold = 16;
    Joint__->hold_multiplier = 0.8;
    Joint__->over_temp_pre_warning = 0;
    Joint__->over_temp_warning = 0;
    Joint__->diag0 = 0;
    Joint__->open_load_B = 0;
    Joint__->open_load_A = 0;
    Joint__->short_2_gnd_B = 0;
    Joint__->short_2_gnd_A = 0;
    Joint__->position_error = 0;
}

/**
 * Инициализация параметров для сустава 6 (оси 6).
 */
void Init_Joint_6(struct MotorStruct *Joint__)
{
    Joint__->standby_position = 32000;
    Joint__->homed_position = 15900;
    Joint__->motor_max_current = MOTOR6_MAX_CURRENT;
    Joint__->motor_max_speed = 0;
    Joint__->motor_max_acceleration = 0;
    Joint__->motor_min_speed = 0;
    Joint__->motor_min_acceleration = 0;
    Joint__->joint_range_positive = 0;
    Joint__->joint_range_negative = 0;
    Joint__->joint_range_positive_steps = 64000;
    Joint__->joint_range_negative_steps = 0;
    Joint__->homed = 0;
    Joint__->homing = 0;
    Joint__->temperature_error = 0;
    Joint__->temperature_warrning = 0;
    Joint__->microstep = MICROSTEP;
    Joint__->limit_switch_trigger = 0;
    Joint__->reduction_ratio = 10;
    Joint__->position = 0;
    Joint__->speed = 0;
    Joint__->current = 0;
    Joint__->LIMIT = LIMIT1;                    // io0 - pg12, -, +
    Joint__->DIR = DIR3;                        // step driver 2 - dir (pg0)
    Joint__->STEP = PUL3;                       // step driver 2 - step(pe4)
    Joint__->SELECT = SELECT3;                  // step driver 2 - cs/uart (pf15)
    Joint__->driver_chip_on_board = 0;
    Joint__->direction_reversed = 1;
    Joint__->irun = 16;
    Joint__->ihold = 16;
    Joint__->hold_multiplier = 0.88;
    Joint__->over_temp_pre_warning = 0;
    Joint__->over_temp_warning = 0;
    Joint__->diag0 = 0;
    Joint__->open_load_B = 0;
    Joint__->open_load_A = 0;
    Joint__->short_2_gnd_B = 0;
    Joint__->short_2_gnd_A = 0;
    Joint__->position_error = 0;
}