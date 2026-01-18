/** @file CAN.h
    @brief Заголовочный файл для утилит CAN/FDCAN на STM32H7.
    
    Содержит определения и прототипы для работы с FDCAN (адаптировано под H7).
*/

#ifndef CAN_UTILS_H
#define CAN_UTILS_H

#include <Arduino.h>
#include <stm32h7xx_hal.h>  // Для HAL на STM32H7

#define DEBUG_CAN 0

typedef enum {
    CAN_50KBPS, 
    CAN_100KBPS, 
    CAN_125KBPS, 
    CAN_250KBPS, 
    CAN_500KBPS, 
    CAN_1000KBPS
} BITRATE;

typedef enum {
    STANDARD_FORMAT = 0, 
    EXTENDED_FORMAT
} CAN_FORMAT;

typedef enum {
    DATA_FRAME = 0, 
    REMOTE_FRAME
} CAN_FRAME;

typedef struct {
    uint32_t id;
    uint8_t  data[8];
    uint8_t  len;
    uint8_t  format;
    uint8_t  type;
} CAN_msg_t;

// Для STM32H7 используется FDCAN
extern FDCAN_HandleTypeDef hfdcan1;

bool CANInit(BITRATE bitrate);
void CANReceive(CAN_msg_t* CAN_rx_msg);
void CANSend(CAN_msg_t* CAN_tx_msg);
uint8_t CANMsgAvail(void);

#endif