#include "structs.h"
#include <TMCStepper.h>
#include <Arduino.h>
#include <AccelStepper.h>
#include "hw_init.h"
#include "iodefs.h"
#include "utils.h"
#include "constants.h"
#include "adc_init.h"
#include "motor_init.h"

// Глобальная структура гриппера
Gripper Comp_gripper;