#include "structs.h"
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
// #include "CAN.h"  // Временно отключено

/// Global structure declaration
Gripper Comp_gripper;

// Временные заглушки для функций, которые могли быть в CAN.h
// Эти функции вызываются из main.cpp
extern "C" {
    // Если эти функции где-то определены, можно оставить их пустыми
    void CAN_protocol(Stream &serial) {
        // Заглушка
    }
    
    void Setup_CAN_bus() {
        // Заглушка
    }
}