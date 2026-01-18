/** @file main.cpp
    @brief Основной файл прошивки для PAROL6 на Mellow Fly Super8.
*/

#include "pins_arduino.h"
#include <TMCStepper.h>
#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
#include "hw_init.h"
#include "iodefs.h"
#include "utils.h"
#include "structs.h"
#include "constants.h"
#include "adc_init.h"
#include "motor_init.h"
#include "stm32h7xx_hal.h"  // Для HAL на STM32H7
#include "coms_CAN.h"  // Для CAN-протокола

#define DEBUG 0  // Режим отладки драйверов
#define DEBUG_COMS 0  // Отладка коммуникаций

// Глобальные переменные
volatile unsigned tick_count;

// Драйверы и шаговики
TMC5160Stepper driver[] = {
    TMC5160Stepper(SELECT1, R_SENSE), 
    TMC5160Stepper(SELECT6, R_SENSE), 
    TMC5160Stepper(SELECT5, R_SENSE),
    TMC5160Stepper(SELECT4, R_SENSE), 
    TMC5160Stepper(SELECT2, R_SENSE), 
    TMC5160Stepper(SELECT3, R_SENSE)
};

AccelStepper stepper[] = {
    AccelStepper(AccelStepper::DRIVER, PUL1, DIR1),
    AccelStepper(AccelStepper::DRIVER, PUL6, DIR6),
    AccelStepper(AccelStepper::DRIVER, PUL5, DIR5),
    AccelStepper(AccelStepper::DRIVER, PUL4, DIR4),
    AccelStepper(AccelStepper::DRIVER, PUL2, DIR2),
    AccelStepper(AccelStepper::DRIVER, PUL3, DIR3)
};

MotorStruct Joint[NUMBER_OF_JOINTS];
Robot PAROL6;

// Время
unsigned long current_time = 0;
unsigned long prev_time = 0;

// Гоминг
int robot_homed = 0;
int homed = 0;
int home_command = 0;

// Входные данные
byte input_byte = 0;
byte start_cond1_byte = 255;
byte start_cond2_byte = 255;
byte start_cond3_byte = 255;
byte end_cond1_byte = 1;
byte end_cond2_byte = 2;

byte start_cond1 = 0;
byte start_cond2 = 0;
byte start_cond3 = 0;
byte good_start = 0;
byte data_len = 0;

byte data_buffer[255];
byte data_counter = 0;

// Оффсет для гриппера
int j5_homing_offset = 8900;

// Этапы гоминга
int run_once = 0;
int joint123_stage1 = 0;
int joint123_stage2 = 0;
int joint123_stage3 = 0;
int joint123_done = 0;

int J4_stage2 = 0;
int J4_stage3 = 0;
int J4_stage4 = 0;
int J4_done = 0;

int J6_stage2 = 0;
int J6_stage3 = 0;
int J6_stage4 = 0;
int J6_done = 0;

int J5_stage2 = 0;
int J5_stage3 = 0;
int J5_stage4 = 0;
int J5_done = 0;

int joint456_stage1 = 0;
int joint456_stage2 = 0;
int joint456_stage3 = 0;

// Демо
int setup_var = 0;
int move1 = 0;
int move2 = 0;

// Прототипы
void Init_motor_drivers(int num);
int home_all();
void Init_motor_direction();
void Unpack_data(uint8_t *data_buffer);
void Pack_data();
void Get_data();
void reset_homing();
void Handle_gripper();

/**
 * Настройка
 */
void setup() {
    HAL_Init();  // Инициализация HAL для STM32H7
    Serial.begin(3000000);  // Baud rate как в оригинале
    while (!Serial) delay(10);
    
    Serial.println("PAROL6 на Mellow Fly Super8 запускается...");

    // Инициализация суставов
    Init_Joint_1(&Joint[0]); // Использует LIMIT6 = io5 - pd6, DIR1 = step driver 0 - dir (pc5), etc.
    Init_Joint_2(&Joint[1]);
    Init_Joint_3(&Joint[2]);
    Init_Joint_4(&Joint[3]);
    Init_Joint_5(&Joint[4]);
    Init_Joint_6(&Joint[5]);

    // Аппаратная инициализация (включает прерывания на лимитах)
    Init_ALL_HW();

    digitalWrite(SUPPLY_ON_OFF, HIGH); // Включить питание (exp1 - pc13)

#if (DEBUG > 0)
    Serial.println("Режим отладки включен");
#endif

    // SPI
    SPI.setMOSI(MOSI);
    SPI.setMISO(MISO);
    SPI.setSCLK(SCK);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.begin();

    // ADC
    MX_ADC1_Init();

    digitalWrite(GLOBAL_ENABLE, LOW);  // Включить драйверы (io6 - pa8, -, +)

    // Инициализация драйверов
    for (int i = 0; i < 6; i++) {
        Init_motor_drivers(i);
    }

    // Настройка шаговиков
    stepper[0].setMaxSpeed(50000);
    stepper[0].setAcceleration(1000);
    stepper[0].setSpeed(0);

    stepper[1].setMaxSpeed(50000);
    stepper[1].setAcceleration(1000);
    stepper[1].setSpeed(0);

    stepper[2].setMaxSpeed(50000);
    stepper[2].setAcceleration(500);
    stepper[2].setSpeed(0);

    stepper[3].setMaxSpeed(50000);
    stepper[3].setAcceleration(500);
    stepper[3].setSpeed(0);

    stepper[4].setMaxSpeed(50000);
    stepper[4].setAcceleration(500);
    stepper[4].setSpeed(0);

    stepper[5].setMaxSpeed(50000);
    stepper[5].setAcceleration(100);
    stepper[5].setSpeed(0);

    // Инициализация CAN
    Setup_CAN_bus();

    // Инициализация направления моторов
    Init_motor_direction();

    Serial.println("Инициализация завершена");
}

/**
 * Основной цикл
 */
void loop() {
    Power_switch_managment();

    // Демо-режим (команда 69)
    if (PAROL6.command == 69) {
        if (setup_var == 0) {
            setup_var = 1;
            move1 = 1;

            stepper[0].setMaxSpeed(6000);
            stepper[0].setAcceleration(6000);
            stepper[0].moveTo(6179);

            stepper[1].setMaxSpeed(4000);
            stepper[1].setAcceleration(4000);
            stepper[1].moveTo(-22222);

            stepper[2].setMaxSpeed(3500);
            stepper[2].setAcceleration(3500);
            stepper[2].moveTo(49812);

            stepper[3].setMaxSpeed(3500);
            stepper[3].setAcceleration(3500);
            stepper[3].moveTo(0);

            stepper[4].setMaxSpeed(3500);
            stepper[4].setAcceleration(3500);
            stepper[4].moveTo(4059);

            stepper[5].setMaxSpeed(7500);
            stepper[5].setAcceleration(7500);
            stepper[5].moveTo(46075);
        }

        if (move1 == 1) {
            for (int i = 0; i < 6; i++) {
                stepper[i].run();
            }

            if (stepper[0].distanceToGo() == 0 && stepper[1].distanceToGo() == 0 && 
                stepper[2].distanceToGo() == 0 && stepper[3].distanceToGo() == 0 &&
                stepper[4].distanceToGo() == 0 && stepper[5].distanceToGo() == 0) {
                move1 = 0;
                move2 = 1;
                
                stepper[0].moveTo(10240);
                stepper[1].moveTo(-32000);
                stepper[2].moveTo(57905);
                stepper[3].moveTo(0);
                stepper[4].moveTo(0);
                stepper[5].moveTo(32000);
            }
        }

        if (move2 == 1) {
            for (int i = 0; i < 6; i++) {
                stepper[i].run();
            }

            if (stepper[0].distanceToGo() == 0 && stepper[1].distanceToGo() == 0 && 
                stepper[2].distanceToGo() == 0 && stepper[3].distanceToGo() == 0 &&
                stepper[4].distanceToGo() == 0 && stepper[5].distanceToGo() == 0) {
                move2 = 0;
            }
        }
    }

    // Обработка команд
    if (PAROL6.command == 255) {  // Пустая команда
        setup_var = 0;
        move1 = 0;
        move2 = 0;
        
        if (home_command == 0) {
            for (int i = 0; i < 6; i++) {
                stepper[i].setSpeed(Joint[i].commanded_velocity);
            }
        }
    }

    if (PAROL6.command == 101) {  // Включить робота
        reset_homing();
        home_command = 0;
        PAROL6.disabled = 0;
    }

    if (PAROL6.command == 102) {  // Отключить робота
        home_command = 0;
        PAROL6.disabled = 1;
    }

    if (PAROL6.command == 103) {  // Очистить ошибки
        home_command = 0;
        reset_homing();
    }

    if (PAROL6.disabled == 0) {
        if (PAROL6.command == 100 || (PAROL6.command == 255 && home_command == 1)) {  // Гоминг
            home_command = 1;
            homed = 0;
            home_all();
        } else if (PAROL6.command == 255 && home_command == 1) {
            home_all();
        }

        if (PAROL6.command == 123) {  // JOG-режим
            home_command = 0;
            homed = 1;
            for (int i = 0; i < 6; i++) {
                stepper[i].setSpeed(Joint[i].commanded_velocity);
                stepper[i].runSpeed();
            }
        }

        if (PAROL6.command == 156) {  // Перейти в позицию
            home_command = 0;
            homed = 1;

            for (int i = 0; i < 6; i++) {
                int speed_set = ((Joint[i].commanded_position - Joint[i].position) / 0.01);
                speed_set = ((Joint[i].commanded_velocity + speed_set) / 2);
                stepper[i].setSpeed(speed_set);
                stepper[i].runSpeed();
            }
        }
    }

    // Обновление позиций и скоростей
    for (int i = 0; i < 6; i++) {
        Joint[i].position = stepper[i].currentPosition();
        Joint[i].speed = stepper[i].speed();
    }

    // Получение данных
    Get_data();

    // Обработка CAN (как в оригинале)
    CAN_protocol(Serial);
}

/**
 * Получение данных из Serial
 */
void Get_data() {
    while (Serial.available() > 0) {
        input_byte = Serial.read();

        if (good_start != 1) {
            if (start_cond1 == 1 && start_cond2 == 1 && start_cond3 == 1) {
                good_start = 1;
                data_len = input_byte;
            }
            
            if (input_byte == start_cond3_byte && start_cond2 == 1 && start_cond1 == 1) {
                start_cond3 = 1;
            } else if (start_cond2 == 1 && start_cond1 == 1) {
                start_cond1 = 0;
                start_cond2 = 0;
            }
            
            if (input_byte == start_cond2_byte && start_cond1 == 1) {
                start_cond2 = 1;
            } else if (start_cond1 == 1) {
                start_cond1 = 0;
            }
            
            if (input_byte == start_cond1_byte) {
                start_cond1 = 1;
            }
        } else {
            data_buffer[data_counter] = input_byte;
            
            if (data_counter == data_len - 1) {
                if (data_buffer[data_len - 2] == end_cond1_byte && 
                    data_buffer[data_len - 1] == end_cond2_byte) {
                    current_time = micros();
                    PAROL6.time_between_commands = current_time - prev_time;
                    prev_time = current_time;
                    
                    Unpack_data(data_buffer);
                    
                    PAROL6.In1 = digitalRead(INPUT1); // exp2 - pb6
                    PAROL6.In2 = digitalRead(INPUT2);
                    PAROL6.Estop = digitalRead(ESTOP);
                    digitalWrite(OUTPUT1, PAROL6.commanded_OUT1);
                    digitalWrite(OUTPUT2, PAROL6.commanded_OUT2);
                    PAROL6.Out1 = PAROL6.commanded_OUT1;
                    PAROL6.Out2 = PAROL6.commanded_OUT2;

                    Handle_gripper();
                    Pack_data();
                }

                good_start = 0;
                start_cond1 = 0;
                start_cond3 = 0;
                start_cond2 = 0;
                data_len = 0;
                data_counter = 0;
            } else {
                data_counter++;
            }
        }
    }
}

/**
 * Обработка гриппера
 */
void Handle_gripper() {
    bool bitArray[8];
    byteToBitsBigEndian(Comp_gripper.command, bitArray);

    if (Comp_gripper.mode == 1) {
        Send_gripper_cal();
    } else if (Comp_gripper.mode == 2) {
        Send_clear_error();
    } else if (Comp_gripper.prev_commanded_position == Comp_gripper.commanded_position &&
               Comp_gripper.prev_commanded_speed == Comp_gripper.commanded_speed &&
               Comp_gripper.prev_commanded_current == Comp_gripper.commanded_current &&
               Comp_gripper.prev_command == Comp_gripper.command &&
               Comp_gripper.prev_commanded_ID == Comp_gripper.commanded_ID &&
               Comp_gripper.mode == 0) {
        Send_gripper_pack_empty();
    } else {
        Send_gripper_pack();
    }

    Comp_gripper.prev_commanded_position = Comp_gripper.commanded_position;
    Comp_gripper.prev_commanded_speed = Comp_gripper.commanded_speed;
    Comp_gripper.prev_commanded_current = Comp_gripper.commanded_current;
    Comp_gripper.prev_command = Comp_gripper.command;
    Comp_gripper.prev_commanded_ID = Comp_gripper.commanded_ID;
}

/**
 * Распаковка данных
 */
void Unpack_data(uint8_t *data_buffer) {
    uint8_t temp[6][3];
    int Joints[6];
    int Speed[6];
    int Command;
    int Affected_joint;
    int InOut;
    int Timeout;
    int Gripper_position;
    int Gripper_speed;
    int Gripper_current;
    int Gripper_command;
    int Gripper_mode;
    int Gripper_ID;
    int CRC_byte;

    int i, j;
    
    for (i = 0, j = 0; i < 18; i += 3, j++) {
        temp[j][0] = data_buffer[i];
        temp[j][1] = data_buffer[i + 1];
        temp[j][2] = data_buffer[i + 2];
        uint8_t buf_test[] = {temp[j][0], temp[j][1], temp[j][2]};
        Joints[j] = bytes_to_int(buf_test);
        Joint[j].commanded_position = Joints[j];
    }
    
    for (i = 18, j = 0; i < 36; i += 3, j++) {
        temp[j][0] = data_buffer[i];
        temp[j][1] = data_buffer[i + 1];
        temp[j][2] = data_buffer[i + 2];
        uint8_t buf_test[] = {temp[j][0], temp[j][1], temp[j][2]};
        Speed[j] = bytes_to_int(buf_test);
        Joint[j].commanded_velocity = Speed[j];
    }

    Command = data_buffer[36];
    PAROL6.command = Command;
    
    Affected_joint = data_buffer[37];
    PAROL6.Affected_joint = Affected_joint;
    
    InOut = data_buffer[38];
    bool bitArray[8];
    byteToBitsBigEndian(InOut, bitArray);
    PAROL6.commanded_OUT1 = bitArray[2];
    PAROL6.commanded_OUT2 = bitArray[3];
    
    Timeout = data_buffer[39];
    PAROL6.Timeout = Timeout;
    
    uint8_t buf_test[2] = {data_buffer[40], data_buffer[41]};
    Gripper_position = two_bytes_to_int(buf_test);
    Comp_gripper.commanded_position = Gripper_position;
    
    buf_test[0] = data_buffer[42];
    buf_test[1] = data_buffer[43];
    Gripper_speed = two_bytes_to_int(buf_test);
    Comp_gripper.commanded_speed = Gripper_speed;
    
    buf_test[0] = data_buffer[44];
    buf_test[1] = data_buffer[45];
    Gripper_current = two_bytes_to_int(buf_test);
    Comp_gripper.commanded_current = Gripper_current;
    
    Gripper_command = data_buffer[46];
    Comp_gripper.command = Gripper_command;
    
    Gripper_mode = data_buffer[47];
    Comp_gripper.mode = Gripper_mode;
    
    Gripper_ID = data_buffer[48];
    Comp_gripper.commanded_ID = Gripper_ID;
    
    CRC_byte = data_buffer[49];
}

/**
 * Упаковка данных для отправки
 */
void Pack_data() {
    uint8_t start_bytes[] = {0xff, 0xff, 0xff};
    int len = 56;
    
    int Position_out[] = {
        Joint[0].position, Joint[1].position, Joint[2].position,
        Joint[3].position, Joint[4].position, Joint[5].position
    };
    
    int Speed_out[] = {
        Joint[0].speed, Joint[1].speed, Joint[2].speed,
        Joint[3].speed, Joint[4].speed, Joint[5].speed
    };
    
    bool Homed[] = {
        (bool)Joint[0].homed, (bool)Joint[1].homed, (bool)Joint[2].homed,
        (bool)Joint[3].homed, (bool)Joint[4].homed, (bool)Joint[5].homed, true, true
    };

    bool IO_var[] = {
        (bool)PAROL6.In1, (bool)PAROL6.In2, (bool)PAROL6.Out1,
        (bool)PAROL6.Out2, (bool)PAROL6.Estop, true, true, true
    };

    bool temp_error[] = {
        (bool)Joint[0].temperature_error, (bool)Joint[1].temperature_error,
        (bool)Joint[2].temperature_error, (bool)Joint[3].temperature_error,
        (bool)Joint[4].temperature_error, (bool)Joint[5].temperature_error, true, true
    };

    bool position_error[] = {
        (bool)Joint[0].position_error, (bool)Joint[1].position_error,
        (bool)Joint[2].position_error, (bool)Joint[3].position_error,
        (bool)Joint[4].position_error, (bool)Joint[5].position_error, true, true
    };
    
    unsigned int timing_data = PAROL6.time_between_commands;
    int timeout_error = PAROL6.timeout_error;
    int xtr2 = PAROL6.command;
    int gripper_ID = Comp_gripper.Gripper_ID;
    int gripper_position = Comp_gripper.current_position;
    int gripper_speed = Comp_gripper.current_speed;
    int gripper_current = Comp_gripper.current_current;
    int gripper_status = Comp_gripper.current_status;
    int object_detection = Comp_gripper.object_detection;
    int CRC_byte = PAROL6.CRC_value;
    uint8_t end_bytes[] = {0x01, 0x02};

    byte data_buffer_send[3];

    Serial.write(start_bytes[0]);
    Serial.write(start_bytes[1]);
    Serial.write(start_bytes[2]);
    Serial.write(len);

    for (int i = 0; i < 6; i++) {
        intTo3Bytes(Position_out[i], data_buffer_send);
        Serial.write(data_buffer_send[0]);
        Serial.write(data_buffer_send[1]);
        Serial.write(data_buffer_send[2]);
    }
    
    for (int i = 0; i < 6; i++) {
        intTo3Bytes(Speed_out[i], data_buffer_send);
        Serial.write(data_buffer_send[0]);
        Serial.write(data_buffer_send[1]);
        Serial.write(data_buffer_send[2]);
    }
    
    Serial.write(bitsToByte(Homed));
    Serial.write(bitsToByte(IO_var));
    Serial.write(bitsToByte(temp_error));
    Serial.write(bitsToByte(position_error));
    
    intTo2Bytes(timing_data, data_buffer_send);
    Serial.write(data_buffer_send[0]);
    Serial.write(data_buffer_send[1]);
    
    Serial.write(timeout_error);
    
    Serial.write(xtr2);
    Serial.write(gripper_ID);
    
    intTo2Bytes(gripper_position, data_buffer_send);
    Serial.write(data_buffer_send[0]);
    Serial.write(data_buffer_send[1]);
    
    intTo2Bytes(gripper_speed, data_buffer_send);
    Serial.write(data_buffer_send[0]);
    Serial.write(data_buffer_send[1]);
    
    intTo2Bytes(gripper_current, data_buffer_send);
    Serial.write(data_buffer_send[0]);
    Serial.write(data_buffer_send[1]);
    
    Serial.write(gripper_status);
    Serial.write(object_detection);
    Serial.write(CRC_byte);
    
    Serial.write(end_bytes[0]);
    Serial.write(end_bytes[1]);
}

/**
 * Сброс этапов гоминга
 */
void reset_homing() {
    run_once = 0;
    joint123_stage1 = 0;
    joint123_stage2 = 0;
    joint123_stage3 = 0;
    joint123_done = 0;

    J4_stage2 = 0;
    J4_stage3 = 0;
    J4_stage4 = 0;
    J4_done = 0;

    J6_stage2 = 0;
    J6_stage3 = 0;
    J6_stage4 = 0;
    J6_done = 0;

    J5_stage2 = 0;
    J5_stage3 = 0;
    J5_stage4 = 0;
    J5_done = 0;

    joint456_stage1 = 0;
    joint456_stage2 = 0;
    joint456_stage3 = 0;
}

/**
 * Гоминг всех суставов (полная реализация как в оригинале)
 */
int home_all() {
    if (homed == 0) {
        if (run_once == 0) {
            for (int i = 0; i < 6; i++) {
                Joint[i].homed = 0;
            }
            
            stepper[0].setSpeed(-2050);
            stepper[1].setSpeed(-2550);
            stepper[2].setSpeed(-2550);
            stepper[3].setSpeed(5550);
            stepper[4].setSpeed(-5050);
            stepper[5].setSpeed(-9550);
            
            run_once = 1;
        }

        if (joint123_done == 0) {
            if (joint123_stage1 == 0) {
                for (int i = 0; i < 3; i++) {
                    stepper[i].runSpeed();
                    if (digitalRead(Joint[i].LIMIT) == Joint[i].limit_switch_trigger) { // Для LIMIT1 - io0 - pg12 и т.д.
                        Joint[i].homing_stage_1 = 1;
                        stepper[i].setSpeed(0);
                        stepper[i].setCurrentPosition(0);
                    }
                }
                
                if (Joint[0].homing_stage_1 && Joint[1].homing_stage_1 && Joint[2].homing_stage_1) {
                    joint123_stage1 = 1;
                    
                    stepper[0].moveTo(850);
                    stepper[0].setSpeed(-2050);
                    stepper[1].moveTo(1050);
                    stepper[1].setSpeed(2050);
                    stepper[2].moveTo(1050);
                    stepper[2].setSpeed(2050);
                }
            }
            
            if (joint123_stage1 == 1 && joint123_stage2 == 0) {
                bool all_moved = true;
                for (int i = 0; i < 3; i++) {
                    stepper[i].runSpeedToPosition();
                    if (stepper[i].distanceToGo() != 0) all_moved = false;
                }
                
                if (all_moved) {
                    joint123_stage2 = 1;
                    stepper[0].setSpeed(-750);
                    stepper[1].setSpeed(-750);
                    stepper[2].setSpeed(-750);
                }
            }
            
            if (joint123_stage2 == 1) {
                for (int i = 0; i < 3; i++) {
                    if (Joint[i].homing_stage_2 == 0) {
                        stepper[i].runSpeed();
                        if (digitalRead(Joint[i].LIMIT) == Joint[i].limit_switch_trigger) {
                            Joint[i].homing_stage_2 = 1;
                            stepper[i].setSpeed(0);
                            stepper[i].setCurrentPosition(0);
                        }
                    }
                }
                
                if (Joint[0].homing_stage_2 && Joint[1].homing_stage_2 && Joint[2].homing_stage_2) {
                    stepper[0].setAcceleration(4500);
                    stepper[0].moveTo(Joint[0].homed_position);
                    stepper[1].setAcceleration(3000);
                    stepper[1].moveTo(Joint[1].homed_position);
                    stepper[2].setAcceleration(5000);
                    stepper[2].moveTo(Joint[2].homed_position);
                    
                    joint123_stage3 = 1;
                }
            }
            
            if (joint123_stage3 == 1 && joint123_done == 0) {
                bool all_at_position = true;
                for (int i = 0; i < 3; i++) {
                    stepper[i].run();
                    if (stepper[i].distanceToGo() != 0) all_at_position = false;
                }
                
                if (all_at_position) {
                    Joint[0].homed = 1;
                    Joint[1].homed = 1;
                    Joint[2].homed = 1;
                    joint123_done = 1;
                }
            }
        }
        
        if (joint123_done == 1) {
            // Сустав 4
            if (Joint[3].homing_stage_2 == 1 && J4_stage4 == 0) {
                stepper[3].setAcceleration(5500);
                stepper[3].moveTo(Joint[3].homed_position);
                stepper[3].run();
                if (stepper[3].currentPosition() == Joint[3].homed_position) {
                    J4_stage4 = 1;
                    stepper[3].setSpeed(2050);
                    J4_done = 1;
                    Joint[3].homed = 1;
                }
            }
            
            if (J4_stage3 == 1 && Joint[3].homing_stage_2 == 0) {
                if (digitalRead(Joint[3].LIMIT) == Joint[3].limit_switch_trigger) {
                    Joint[3].homing_stage_2 = 1;
                    stepper[3].setSpeed(0);
                    stepper[3].setCurrentPosition(0);
                }
                stepper[3].runSpeed();
            }
            
            if (Joint[3].homing_stage_1 == 1 && J4_stage2 == 0) {
                stepper[3].moveTo(-1550);
                stepper[3].setSpeed(-4050);
                stepper[3].runSpeedToPosition();
                if (stepper[3].currentPosition() == -1550) {
                    J4_stage2 = 1;
                    J4_stage3 = 1;
                    stepper[3].setSpeed(4050);
                }
            }
            
            if (Joint[3].homing_stage_1 == 0) {
                if (digitalRead(Joint[3].LIMIT) == Joint[3].limit_switch_trigger) {
                    Joint[3].homing_stage_1 = 1;
                    stepper[3].setSpeed(0);
                    stepper[3].setCurrentPosition(0);
                }
                stepper[3].runSpeed();
            }
        }
        
        if (J4_done == 1) {
            // Сустав 6
            if (Joint[5].homing_stage_2 == 1 && J6_stage4 == 0) {
                stepper[5].setAcceleration(10500);
                stepper[5].moveTo(Joint[5].homed_position);
                stepper[5].run();
                if (stepper[5].currentPosition() == Joint[5].homed_position) {
                    J6_stage4 = 1;
                    stepper[5].setSpeed(1050);
                    J6_done = 1;
                    Joint[5].homed = 1;
                }
            }
            
            if (J6_stage3 == 1 && Joint[5].homing_stage_2 == 0) {
                if (digitalRead(Joint[5].LIMIT) == Joint[5].limit_switch_trigger) {
                    Joint[5].homing_stage_2 = 1;
                    stepper[5].setSpeed(0);
                    stepper[5].setCurrentPosition(0);
                }
                stepper[5].runSpeed();
            }
            
            if (Joint[5].homing_stage_1 == 1 && J6_stage2 == 0) {
                stepper[5].moveTo(1550);
                stepper[5].setSpeed(4050);
                stepper[5].runSpeedToPosition();
                if (stepper[5].currentPosition() == 1550) {
                    J6_stage2 = 1;
                    J6_stage3 = 1;
                    stepper[5].setSpeed(-8050);
                }
            }
            
            if (Joint[5].homing_stage_1 == 0) {
                if (digitalRead(Joint[5].LIMIT) == Joint[5].limit_switch_trigger) {
                    Joint[5].homing_stage_1 = 1;
                    stepper[5].setSpeed(0);
                    stepper[5].setCurrentPosition(0);
                }
                stepper[5].runSpeed();
            }
        }
        
        if (J6_done == 1) {
            // Сустав 5
            if (Joint[4].homing_stage_2 == 1 && J5_stage4 == 0) {
                stepper[4].setAcceleration(5500);
                stepper[4].moveTo(j5_homing_offset);
                stepper[5].setAcceleration(5500);
                stepper[5].moveTo(0);
                stepper[4].run();
                stepper[5].run();
                if (stepper[4].currentPosition() == j5_homing_offset && stepper[5].currentPosition() == 0) {
                    J5_stage4 = 1;
                    stepper[4].setSpeed(1050);
                    J5_done = 1;
                    Joint[4].homed = 1;
                    Joint[5].homed = 1;
                }
            }
            
            if (J5_stage3 == 1 && Joint[4].homing_stage_2 == 0) {
                if (digitalRead(Joint[4].LIMIT) == Joint[4].limit_switch_trigger) {
                    Joint[4].homing_stage_2 = 1;
                    stepper[4].setSpeed(0);
                    stepper[4].setCurrentPosition(0);
                }
                stepper[4].runSpeed();
            }
            
            if (Joint[4].homing_stage_1 == 1 && J5_stage2 == 0) {
                stepper[4].moveTo(1550);
                stepper[4].setSpeed(4050);
                stepper[4].runSpeedToPosition();
                if (stepper[4].currentPosition() == 1550) {
                    J5_stage2 = 1;
                    J5_stage3 = 1;
                    stepper[4].setSpeed(-4050);
                }
            }
            
            if (Joint[4].homing_stage_1 == 0) {
                if (digitalRead(Joint[4].LIMIT) == Joint[4].limit_switch_trigger) {
                    Joint[4].homing_stage_1 = 1;
                    stepper[4].setSpeed(0);
                    stepper[4].setCurrentPosition(0);
                }
                stepper[4].runSpeed();
            }
        }
        
        if (J5_done == 1) {
            for (int i = 0; i < 6; i++) {
                stepper[i].setCurrentPosition(Joint[i].standby_position);
                Joint[i].homing_stage_1 = 0;
                Joint[i].homing_stage_2 = 0;
            }

            reset_homing();
            homed = 1;
        }
    }
    
    return homed;
}

/**
 * Инициализация направления моторов
 */
void Init_motor_direction() {
    for (int i = 0; i < 6; i++) {
        stepper[i].setPinsInverted(Joint[i].direction_reversed, false, false);
    }
}

/**
 * Инициализация драйвера мотора
 */
void Init_motor_drivers(int num) {
    digitalWrite(Joint[num].SELECT, HIGH);
    
    driver[num].begin();
    driver[num].rms_current(Joint[num].motor_max_current * 0.85, Joint[num].hold_multiplier);
    driver[num].en_pwm_mode(1);
    driver[num].toff(4);
    driver[num].blank_time(24);
    driver[num].pwm_autoscale(1);
    driver[num].microsteps(Joint[num].microstep);

    Joint[num].current = driver[num].rms_current();
    Joint[num].hold_multiplier = driver[num].hold_multiplier();
    Joint[num].ihold = driver[num].ihold();
    Joint[num].irun = driver[num].irun();
    Joint[num].open_load_B = driver[num].olb();
    Joint[num].open_load_A = driver[num].ola();
    Joint[num].short_2_gnd_B = driver[num].s2gb();
    Joint[num].short_2_gnd_A = driver[num].s2ga();
    Joint[num].over_temp_pre_warning = driver[num].otpw();
    Joint[num].over_temp_warning = driver[num].ot();
    Joint[num].diag0 = driver[num].diag0_error();

#if (DEBUG > 0)
    Serial.print("Сустав ");
    Serial.print(num + 1);
    Serial.print(": Ток=");
    Serial.print(Joint[num].current);
    Serial.print("mA, Соединение=");
    Serial.println(driver[num].test_connection());
#endif

    digitalWrite(Joint[num].SELECT, LOW);
}