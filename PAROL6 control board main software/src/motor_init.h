/** @file motor_init.h (заголовочный файл motor_init.h)
    @brief Документированный файл.
    
    Заголовочный файл инициализации двигателей.
    Для схемы обратитесь к:
    Для дополнительной функциональности выводов обратитесь к:

*/

#ifndef MOTOR_INIT
#define MOTOR_INIT

#include "structs.h"
#include "hw_init.h"
#include "common.h"
#include "iodefs.h"
#include "constants.h"
/// ЗДЕСЬ ИНИЦИАЛИЗИРУЕМ СТРУКТУРЫ?? (HERE INIT STRUCTURES??)

/* Прототипы функций для инициализации структур двигателей/суставов */

void Init_motor_struct(struct MotorStruct *Joint__);
void Init_Joint_1(struct MotorStruct *Joint__);
void Init_Joint_2(struct MotorStruct *Joint__);
void Init_Joint_3(struct MotorStruct *Joint__);
void Init_Joint_4(struct MotorStruct *Joint__);
void Init_Joint_5(struct MotorStruct *Joint__);
void Init_Joint_6(struct MotorStruct *Joint__);


#endif /* MOTOR_INIT */