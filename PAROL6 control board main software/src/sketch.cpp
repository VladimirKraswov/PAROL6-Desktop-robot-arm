/** @file main.cpp
    @brief Main file of PAROL6 mainboard firmware.
    Author: Petar Crnjak
*/

#include "pins_arduino.h"
#include <TMCStepper.h>
#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
#include "hw_init.h"
#include "iodefs.h"
#include "utils.h"
#include "common.h"
#include "constants.h"
#include "adc_init.h"
#include "motor_init.h"
// Удаляем проблемные CAN заголовки - временно отключаем CAN
// #include "CAN.h"
// #include "coms_CAN.h"

// Прототипы функций для заглушек (временно)
void Send_gripper_cal();
void Send_clear_error();
void Send_gripper_pack_empty();
void Send_gripper_pack();

#define DEBUG 0      // For stepper drivers debug
#define DEBUG_COMS 0 // comms debug

// Global variables
volatile unsigned tick_count;

// Init structures
TMC5160Stepper driver[] = {
    TMC5160Stepper(SELECT1, R_SENSE), 
    TMC5160Stepper(SELECT6, R_SENSE), 
    TMC5160Stepper(SELECT5, R_SENSE),
    TMC5160Stepper(SELECT4, R_SENSE), 
    TMC5160Stepper(SELECT2, R_SENSE), 
    TMC5160Stepper(SELECT3, R_SENSE)
};

AccelStepper stepper[] = {
    AccelStepper(AccelStepper::DRIVER, PUL1, DIR1),  // Для режима драйвера STEP/DIR
    AccelStepper(AccelStepper::DRIVER, PUL6, DIR6),
    AccelStepper(AccelStepper::DRIVER, PUL5, DIR5),
    AccelStepper(AccelStepper::DRIVER, PUL4, DIR4),
    AccelStepper(AccelStepper::DRIVER, PUL2, DIR2),
    AccelStepper(AccelStepper::DRIVER, PUL3, DIR3)
};

struct MotorStruct Joint[NUMBER_OF_JOINTS];
struct Robot PAROL6;

// Variables for measuring elapsed time
unsigned long current_time = 0;
unsigned long prev_time = 0;

// Variables for robot homing
int robot_homed = 0;
int homed = 0;
int home_command = 0;

// Input helper variables
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

// FOR PNEUMATIC GRIPPER 8900
int j5_homing_offset = 8900; // for SSG48 GRIPPER 8035

// Home commands
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

// Demo stage test
int setup_var = 0;
int move1 = 0;
int move2 = 0;

// Function prototypes
void Init_motor_drivers(int num);
int home_all();
void Init_motor_direction();
void Unpack_data(uint8_t *data_buffer);
void Pack_data();
void Get_data();
void reset_homing();
void Handle_gripper();

// Заглушки для функций gripper (временные)
void Send_gripper_cal() {
    // Заглушка
}

void Send_clear_error() {
    // Заглушка
}

void Send_gripper_pack_empty() {
    // Заглушка
}

void Send_gripper_pack() {
    // Заглушка
}

void setup()
{
    // Initialize serial
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("PAROL6 Robot Controller Starting...");
    Serial.println("Mellow Fly 3D Super8 (STM32H723)");

    // Init Joint structures
    Init_Joint_1(&Joint[0]);
    Init_Joint_2(&Joint[1]);
    Init_Joint_3(&Joint[2]);
    Init_Joint_4(&Joint[3]);
    Init_Joint_5(&Joint[4]);
    Init_Joint_6(&Joint[5]);

    // Init hardware
    Init_motor_direction();
    Init_Digital_Inputs();
    Init_Digital_Outputs();

    // Enable power supply
    digitalWrite(SUPPLY_ON_OFF, HIGH);

    // Disable stepper drivers initially
    digitalWrite(GLOBAL_ENABLE, HIGH);

#if (DEBUG > 0)
    delay(3000);
    Serial.println("Debug mode enabled");
#endif

    // Initialize SPI
    SPI.setMOSI(MOSI);
    SPI.setMISO(MISO);
    SPI.setSCLK(SCK);
    SPI.setClockDivider(SPI_CLOCK_DIV8); // Safer frequency
    SPI.begin();

    delay(200); // Needed for power switch management

    // Initialize ADC
    MX_ADC1_Init();

    // Enable stepper drivers
    digitalWrite(GLOBAL_ENABLE, LOW);

    // Initialize motor drivers
    for (int i = 0; i < 6; i++)
    {
        Init_motor_drivers(i);
        delay(90);
    }

    // Configure steppers
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

    // Initialize CAN bus (временно отключено)
    // Setup_CAN_bus();

    Serial.println("Initialization complete");
}

void loop()
{
    // Handle power switch
    Power_switch_managment();

    // Demo sequence
    if (PAROL6.command == 69)
    {
        if (setup_var == 0)
        {
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

        if (move1 == 1)
        {
            for (int i = 0; i < 6; i++)
            {
                stepper[i].run();
            }

            if (stepper[0].distanceToGo() == 0 && stepper[1].distanceToGo() == 0 && 
                stepper[2].distanceToGo() == 0 && stepper[3].distanceToGo() == 0 &&
                stepper[4].distanceToGo() == 0 && stepper[5].distanceToGo() == 0)
            {
                move1 = 0;
                move2 = 1;
                
                stepper[0].moveTo(10240);
                stepper[1].moveTo(-32000);
                stepper[2].moveTo(57905);
                stepper[3].moveTo(0);
                stepper[4].moveTo(0);
                stepper[5].moveTo(32000);
                delay(2000);
            }
        }

        if (move2 == 1)
        {
            for (int i = 0; i < 6; i++)
            {
                stepper[i].run();
            }

            if (stepper[0].distanceToGo() == 0 && stepper[1].distanceToGo() == 0 && 
                stepper[2].distanceToGo() == 0 && stepper[3].distanceToGo() == 0 &&
                stepper[4].distanceToGo() == 0 && stepper[5].distanceToGo() == 0)
            {
                move2 = 0;
            }
        }
    }

    // Handle commands
    if (PAROL6.command == 255) // Dummy command
    {
        setup_var = 0;
        move1 = 0;
        move2 = 0;
        
        if (home_command == 0)
        {
            for (int i = 0; i < 6; i++)
            {
                stepper[i].setSpeed(Joint[i].commanded_velocity);
            }
        }
    }

    // Enable robot
    if (PAROL6.command == 101)
    {
        reset_homing();
        home_command = 0;
        PAROL6.disabled = 0;
    }

    // Disable robot
    if (PAROL6.command == 102)
    {
        home_command = 0;
        PAROL6.disabled = 1;
    }

    // Clear error
    if (PAROL6.command == 103)
    {
        home_command = 0;
        reset_homing();
    }

    // Handle motion commands if robot is enabled
    if (PAROL6.disabled == 0)
    {
        // Home robot
        if (PAROL6.command == 100)
        {
            home_command = 1;
            homed = 0;
            if (homed == 0)
            {
                home_all();
            }
            else
            {
                home_command = 0;
            }
        }
        else if (PAROL6.command == 255 && home_command == 1)
        {
            if (homed == 0)
            {
                home_all();
            }
            else
            {
                home_command = 0;
            }
        }

        // JOG command
        if (PAROL6.command == 123)
        {
            home_command = 0;
            homed = 1;
            for (int i = 0; i < 6; i++)
            {
                stepper[i].setSpeed(Joint[i].commanded_velocity);
                stepper[i].runSpeed();
            }
        }

        // GO TO POSITION command
        if (PAROL6.command == 156)
        {
            home_command = 0;
            homed = 1;

            for (int i = 0; i < 6; i++)
            {
                int speed_set = int(((Joint[i].commanded_position - Joint[i].position) / 0.01));
                speed_set = int(((Joint[i].commanded_velocity + speed_set) / 2));
                stepper[i].setSpeed(speed_set);
                stepper[i].runSpeed();
            }
        }
    }

    // Update position and speed
    for (int i = 0; i < 6; i++)
    {
        Joint[i].position = stepper[i].currentPosition();
        Joint[i].speed = stepper[i].speed();
    }

    // Process incoming data
    Get_data();
}

void Get_data()
{
    // Process CAN messages (временно отключено)
    // CAN_protocol(Serial);

    // Process serial data
    while (Serial.available() > 0)
    {
        input_byte = Serial.read();

        if (good_start != 1)
        {
            // Check for start bytes
            if (start_cond1 == 1 && start_cond2 == 1 && start_cond3 == 1)
            {
                good_start = 1;
                data_len = input_byte;
            }
            
            if (input_byte == start_cond3_byte && start_cond2 == 1 && start_cond1 == 1)
            {
                start_cond3 = 1;
            }
            else if (start_cond2 == 1 && start_cond1 == 1)
            {
                start_cond1 = 0;
                start_cond2 = 0;
            }
            
            if (input_byte == start_cond2_byte && start_cond1 == 1)
            {
                start_cond2 = 1;
            }
            else if (start_cond1 == 1)
            {
                start_cond1 = 0;
            }
            
            if (input_byte == start_cond1_byte)
            {
                start_cond1 = 1;
            }
        }
        else
        {
            // Store data after good start
            data_buffer[data_counter] = input_byte;
            
            if (data_counter == data_len - 1)
            {
                // Check for end bytes
                if (data_buffer[data_len - 2] == end_cond1_byte && 
                    data_buffer[data_len - 1] == end_cond2_byte)
                {
                    current_time = micros();
                    PAROL6.time_between_commands = current_time - prev_time;
                    prev_time = current_time;
                    
                    Unpack_data(data_buffer);
                    
                    // Read inputs and set outputs
                    PAROL6.In1 = digitalRead(INPUT1);
                    PAROL6.In2 = digitalRead(INPUT2);
                    PAROL6.Estop = digitalRead(ESTOP);
                    digitalWrite(OUTPUT1, PAROL6.commanded_OUT1);
                    digitalWrite(OUTPUT2, PAROL6.commanded_OUT2);
                    PAROL6.Out1 = PAROL6.commanded_OUT1;
                    PAROL6.Out2 = PAROL6.commanded_OUT2;

                    Handle_gripper();
                    Pack_data();
                }

                // Reset for next packet
                good_start = 0;
                start_cond1 = 0;
                start_cond3 = 0;
                start_cond2 = 0;
                data_len = 0;
                data_counter = 0;
            }
            else
            {
                data_counter++;
            }
        }
    }
}

void Handle_gripper()
{
    bool bitArray[8];
    byteToBitsBigEndian(Comp_gripper.command, bitArray);

    if (Comp_gripper.mode == 1)
    {
        Send_gripper_cal();
    }
    else if (Comp_gripper.mode == 2)
    {
        Send_clear_error();
    }
    else if (Comp_gripper.prev_commanded_position == Comp_gripper.commanded_position &&
             Comp_gripper.prev_commanded_speed == Comp_gripper.commanded_speed &&
             Comp_gripper.prev_commanded_current == Comp_gripper.commanded_current &&
             Comp_gripper.prev_command == Comp_gripper.command &&
             Comp_gripper.prev_commanded_ID == Comp_gripper.commanded_ID &&
             Comp_gripper.mode == 0)
    {
        Send_gripper_pack_empty();
    }
    else
    {
        Send_gripper_pack();
    }

    // Save current commands as previous
    Comp_gripper.prev_commanded_position = Comp_gripper.commanded_position;
    Comp_gripper.prev_commanded_speed = Comp_gripper.commanded_speed;
    Comp_gripper.prev_commanded_current = Comp_gripper.commanded_current;
    Comp_gripper.prev_command = Comp_gripper.command;
    Comp_gripper.prev_commanded_ID = Comp_gripper.commanded_ID;
}

void Unpack_data(uint8_t *data_buffer)
{
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
    
    // Unpack position data (18 bytes)
    for (i = 0, j = 0; i < 18; i += 3, j++)
    {
        temp[j][0] = static_cast<uint8_t>(data_buffer[i]);
        temp[j][1] = static_cast<uint8_t>(data_buffer[i + 1]);
        temp[j][2] = static_cast<uint8_t>(data_buffer[i + 2]);
        uint8_t buf_test[] = {temp[j][0], temp[j][1], temp[j][2]};
        Joints[j] = bytes_to_int(buf_test);
        Joint[j].commanded_position = Joints[j];
    }
    
    // Unpack speed data (18 bytes)
    for (i = 18, j = 0; i < 36; i += 3, j++)
    {
        temp[j][0] = static_cast<uint8_t>(data_buffer[i]);
        temp[j][1] = static_cast<uint8_t>(data_buffer[i + 1]);
        temp[j][2] = static_cast<uint8_t>(data_buffer[i + 2]);
        uint8_t buf_test[] = {temp[j][0], temp[j][1], temp[j][2]};
        Speed[j] = bytes_to_int(buf_test);
        Joint[j].commanded_velocity = Speed[j];
    }

    // Unpack remaining data
    Command = static_cast<int>(data_buffer[36]);
    PAROL6.command = Command;
    
    Affected_joint = static_cast<int>(data_buffer[37]);
    PAROL6.Affected_joint = Affected_joint;
    
    InOut = static_cast<int>(data_buffer[38]);
    bool bitArray[8];
    byteToBitsBigEndian(InOut, bitArray);
    PAROL6.commanded_OUT1 = bitArray[2];
    PAROL6.commanded_OUT2 = bitArray[3];
    
    Timeout = static_cast<int>(data_buffer[39]);
    PAROL6.Timeout = Timeout;
    
    uint8_t buf_test[2] = {static_cast<uint8_t>(data_buffer[40]), 
                          static_cast<uint8_t>(data_buffer[41])};
    Gripper_position = two_bytes_to_int(buf_test);
    Comp_gripper.commanded_position = Gripper_position;
    
    buf_test[0] = static_cast<uint8_t>(data_buffer[42]);
    buf_test[1] = static_cast<uint8_t>(data_buffer[43]);
    Gripper_speed = two_bytes_to_int(buf_test);
    Comp_gripper.commanded_speed = Gripper_speed;
    
    buf_test[0] = static_cast<uint8_t>(data_buffer[44]);
    buf_test[1] = static_cast<uint8_t>(data_buffer[45]);
    Gripper_current = two_bytes_to_int(buf_test);
    Comp_gripper.commanded_current = Gripper_current;
    
    Gripper_command = static_cast<int>(data_buffer[46]);
    Comp_gripper.command = Gripper_command;
    
    Gripper_mode = static_cast<int>(data_buffer[47]);
    Comp_gripper.mode = Gripper_mode;
    
    Gripper_ID = static_cast<int>(data_buffer[48]);
    Comp_gripper.commanded_ID = Gripper_ID;
    
    CRC_byte = static_cast<int>(data_buffer[49]);
}

void Pack_data()
{
    // Start bytes and length
    uint8_t start_bytes[] = {0xff, 0xff, 0xff};
    int len = 56;
    
    // Prepare data
    int Position_out[] = {
        Joint[0].position, Joint[1].position, Joint[2].position,
        Joint[3].position, Joint[4].position, Joint[5].position
    };
    
    int Speed_out[] = {
        Joint[0].speed, Joint[1].speed, Joint[2].speed,
        Joint[3].speed, Joint[4].speed, Joint[5].speed
    };
    
    bool Homed[] = {
        static_cast<bool>(Joint[0].homed), 
        static_cast<bool>(Joint[1].homed), 
        static_cast<bool>(Joint[2].homed),
        static_cast<bool>(Joint[3].homed), 
        static_cast<bool>(Joint[4].homed), 
        static_cast<bool>(Joint[5].homed), 
        true, true
    };

    bool IO_var[] = {
        static_cast<bool>(PAROL6.In1), 
        static_cast<bool>(PAROL6.In2), 
        static_cast<bool>(PAROL6.Out1),
        static_cast<bool>(PAROL6.Out2), 
        static_cast<bool>(PAROL6.Estop), 
        true, true, true
    };

    bool temp_error[] = {
        static_cast<bool>(Joint[0].temperature_error), 
        static_cast<bool>(Joint[1].temperature_error),
        static_cast<bool>(Joint[2].temperature_error), 
        static_cast<bool>(Joint[3].temperature_error),
        static_cast<bool>(Joint[4].temperature_error), 
        static_cast<bool>(Joint[5].temperature_error), 
        true, true
    };

    bool position_error[] = {
        static_cast<bool>(Joint[0].position_error), 
        static_cast<bool>(Joint[1].position_error),
        static_cast<bool>(Joint[2].position_error), 
        static_cast<bool>(Joint[3].position_error),
        static_cast<bool>(Joint[4].position_error), 
        static_cast<bool>(Joint[5].position_error), 
        true, true
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

    // Send start bytes
    Serial.write(start_bytes[0]);
    Serial.write(start_bytes[1]);
    Serial.write(start_bytes[2]);
    Serial.write(len);

    // Position data
    for (int i = 0; i < 6; i++)
    {
        intTo3Bytes(Position_out[i], data_buffer_send);
        Serial.write(data_buffer_send[0]);
        Serial.write(data_buffer_send[1]);
        Serial.write(data_buffer_send[2]);
    }
    
    // Speed data
    for (int i = 0; i < 6; i++)
    {
        intTo3Bytes(Speed_out[i], data_buffer_send);
        Serial.write(data_buffer_send[0]);
        Serial.write(data_buffer_send[1]);
        Serial.write(data_buffer_send[2]);
    }
    
    // Other data
    Serial.write(bitsToByte(Homed));
    Serial.write(bitsToByte(IO_var));
    Serial.write(bitsToByte(temp_error));
    Serial.write(bitsToByte(position_error));
    
    // Timing data
    intTo2Bytes(timing_data, data_buffer_send);
    Serial.write(data_buffer_send[0]);
    Serial.write(data_buffer_send[1]);
    
    // Timeout error
    Serial.write(timeout_error);
    
    // Additional data
    Serial.write(xtr2);
    Serial.write(gripper_ID);
    
    // Gripper data
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
    
    // End bytes
    Serial.write(end_bytes[0]);
    Serial.write(end_bytes[1]);
}

void reset_homing()
{
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

int home_all()
{
    if (homed == 0)
    {
        if (run_once == 0)
        {
            for (int i = 0; i < 6; i++)
            {
                Joint[i].homed = 0;
            }
            
            // Set initial homing speeds
            stepper[0].setSpeed(-2050);
            stepper[1].setSpeed(-2550);
            stepper[2].setSpeed(-2550);
            stepper[3].setSpeed(5550);
            stepper[4].setSpeed(-5050);
            stepper[5].setSpeed(-9550);
            
            run_once = 1;
        }

        if (joint123_done == 0)
        {
            // Homing joints 1, 2, 3
            // Stage 1: Find limits
            if (joint123_stage1 == 0)
            {
                // Move until limits are hit
                for (int i = 0; i < 3; i++)
                {
                    stepper[i].runSpeed();
                    if (digitalRead(Joint[i].LIMIT) == Joint[i].limit_switch_trigger)
                    {
                        Joint[i].homing_stage_1 = 1;
                        stepper[i].setSpeed(0);
                        stepper[i].setCurrentPosition(0);
                    }
                }
                
                // Check if all limits found
                if (Joint[0].homing_stage_1 && Joint[1].homing_stage_1 && Joint[2].homing_stage_1)
                {
                    joint123_stage1 = 1;
                    
                    // Move away from limits
                    stepper[0].moveTo(850);
                    stepper[0].setSpeed(-2050);
                    stepper[1].moveTo(1050);
                    stepper[1].setSpeed(2050);
                    stepper[2].moveTo(1050);
                    stepper[2].setSpeed(2050);
                }
            }
            
            // Stage 2: Move to reference positions
            if (joint123_stage1 == 1 && joint123_stage2 == 0)
            {
                bool all_moved = true;
                for (int i = 0; i < 3; i++)
                {
                    stepper[i].runSpeedToPosition();
                    if (stepper[i].distanceToGo() != 0)
                        all_moved = false;
                }
                
                if (all_moved)
                {
                    joint123_stage2 = 1;
                    // Move slowly to find limits again
                    stepper[0].setSpeed(-750);
                    stepper[1].setSpeed(-750);
                    stepper[2].setSpeed(-750);
                }
            }
            
            // Stage 3: Final homing
            if (joint123_stage2 == 1)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (Joint[i].homing_stage_2 == 0)
                    {
                        stepper[i].runSpeed();
                        if (digitalRead(Joint[i].LIMIT) == Joint[i].limit_switch_trigger)
                        {
                            Joint[i].homing_stage_2 = 1;
                            stepper[i].setSpeed(0);
                            stepper[i].setCurrentPosition(0);
                        }
                    }
                }
                
                // Check if all joints are homed
                if (Joint[0].homing_stage_2 && Joint[1].homing_stage_2 && Joint[2].homing_stage_2)
                {
                    // Move to standby positions
                    stepper[0].setAcceleration(4500);
                    stepper[0].moveTo(Joint[0].homed_position);
                    stepper[1].setAcceleration(3000);
                    stepper[1].moveTo(Joint[1].homed_position);
                    stepper[2].setAcceleration(5000);
                    stepper[2].moveTo(Joint[2].homed_position);
                    
                    joint123_stage3 = 1;
                }
            }
            
            // Stage 4: Move to final positions
            if (joint123_stage3 == 1 && joint123_done == 0)
            {
                bool all_at_position = true;
                for (int i = 0; i < 3; i++)
                {
                    stepper[i].run();
                    if (stepper[i].distanceToGo() != 0)
                        all_at_position = false;
                }
                
                if (all_at_position)
                {
                    Joint[0].homed = 1;
                    Joint[1].homed = 1;
                    Joint[2].homed = 1;
                    joint123_done = 1;
                }
            }
        }
        
        // Continue with other joints (4, 5, 6)...
        // (Implementation would continue similarly)
    }

    // Homing complete
    if (J5_done == 1)
    {
        for (int i = 0; i < 6; i++)
        {
            stepper[i].setCurrentPosition(Joint[i].standby_position);
            Joint[i].homing_stage_1 = 0;
            Joint[i].homing_stage_2 = 0;
        }

        reset_homing();
        homed = 1;
    }
    
    return homed;
}

void Init_motor_direction() {
    for (int i = 0; i < 6; i++) {
        stepper[i].setPinsInverted(Joint[i].direction_reversed, false, false);
    }
}

void Init_motor_drivers(int num)
{
    digitalWrite(Joint[num].SELECT, HIGH);
    
    driver[num].begin();
    driver[num].rms_current(Joint[num].motor_max_current * 0.85, Joint[num].hold_multiplier);
    driver[num].en_pwm_mode(1);
    driver[num].toff(4);
    driver[num].blank_time(24);
    driver[num].pwm_autoscale(1);
    driver[num].microsteps(Joint[num].microstep);

    // Read driver status
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
    Serial.print("Joint ");
    Serial.print(num + 1);
    Serial.print(": Current=");
    Serial.print(Joint[num].current);
    Serial.print("mA, Connection=");
    Serial.println(driver[num].test_connection());
#endif

    digitalWrite(Joint[num].SELECT, LOW);
}