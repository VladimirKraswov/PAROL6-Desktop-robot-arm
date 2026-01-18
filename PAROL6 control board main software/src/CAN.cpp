/** @file CAN.cpp
    @brief Реализация утилит FDCAN для STM32H7.
    
    Содержит функции для инициализации, отправки и приема сообщений по FDCAN в классическом режиме.
*/

#include "CAN.h"

// Дескриптор FDCAN (для H7)
FDCAN_HandleTypeDef hfdcan1;
// Фильтр (принимаем все сообщения)
FDCAN_FilterTypeDef can_filter;

bool CANInit(BITRATE bitrate) {
    // 1. Настройка пинов CAN (PB8/RX, PB9/TX для FDCAN1 на Mellow Fly Super8)
    // Предполагаем стандартные пины: PB8 - RX, PB9 - TX
    pinMode(PB8, INPUT_PULLUP);  // RX
    pinMode(PB9, OUTPUT);        // TX (AF7 для FDCAN1)

    // 2. Настройка скорости (прескалера для FDCAN)
    uint32_t prescaler = 0;
    uint32_t ts1 = 13;  // Time segment 1 (bs1 - 1)
    uint32_t ts2 = 2;   // Time segment 2 (bs2 - 1)
    uint32_t sjw = 1;   // Sync jump width
    switch(bitrate) {
        case CAN_1000KBPS: prescaler = 5; break;  // Пример для fdcan_ck ≈ 80MHz: 80 / 5 / (1+13+2) = 1Mbps
        case CAN_500KBPS:  prescaler = 10; break;
        case CAN_250KBPS:  prescaler = 20; break;
        case CAN_125KBPS:  prescaler = 40; break;
        case CAN_100KBPS:  prescaler = 50; break;
        case CAN_50KBPS:   prescaler = 100; break;
        default: prescaler = 5; // 1000 kbps по умолчанию
    }

    // 3. Базовая инициализация FDCAN в классическом режиме
    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;  // Классический CAN
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = ENABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = ENABLE;
    hfdcan1.Init.NominalPrescaler = prescaler;
    hfdcan1.Init.NominalSyncJumpWidth = sjw;
    hfdcan1.Init.NominalTimeSeg1 = ts1;
    hfdcan1.Init.NominalTimeSeg2 = ts2;
    hfdcan1.Init.DataPrescaler = prescaler;  // Для классического режима
    hfdcan1.Init.DataSyncJumpWidth = sjw;
    hfdcan1.Init.DataTimeSeg1 = ts1;
    hfdcan1.Init.DataTimeSeg2 = ts2;
    hfdcan1.Init.StdFiltersNbr = 1;  // 1 стандартный фильтр
    hfdcan1.Init.ExtFiltersNbr = 0;
    hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK) {
        #if (DEBUG_CAN > 0)
        Serial.println("FDCAN HAL Init Failed!");
        #endif
        return false;
    }

    // 4. Настройка фильтра (принимаем все стандартные ID)
    can_filter.IdType = FDCAN_STANDARD_ID;
    can_filter.FilterIndex = 0;
    can_filter.FilterType = FDCAN_FILTER_MASK;
    can_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    can_filter.FilterID1 = 0x000;  // ID
    can_filter.FilterID2 = 0x7FF;  // Маска (все ID)

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &can_filter) != HAL_OK) {
        #if (DEBUG_CAN > 0)
        Serial.println("FDCAN Filter Config Failed!");
        #endif
        return false;
    }

    // 5. Запуск FDCAN
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
        #if (DEBUG_CAN > 0)
        Serial.println("FDCAN Start Failed!");
        #endif
        return false;
    }

    // Активация прерываний (если нужно)
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    #if (DEBUG_CAN > 0)
    Serial.println("FDCAN Initialized (HAL, Classic Mode)");
    #endif
    return true;
}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* hfdcan) {
  if(hfdcan->Instance == FDCAN1) {
    __HAL_RCC_FDCAN_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void CANSend(CAN_msg_t* CAN_tx_msg) {
    FDCAN_TxHeaderTypeDef tx_header;
    uint32_t tx_mailbox;

    tx_header.Identifier = CAN_tx_msg->id;
    tx_header.IdType = FDCAN_STANDARD_ID;  // Стандартный 11-бит ID
    tx_header.TxFrameType = (CAN_tx_msg->type == REMOTE_FRAME) ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME;
    tx_header.DataLength = CAN_tx_msg->len << 16;  // DLC для len <=8
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;  // Классический режим
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_header, CAN_tx_msg->data) != HAL_OK) {
        #if (DEBUG_CAN > 0)
        Serial.println("FDCAN Send Failed");
        #endif
    } else {
        #if (DEBUG_CAN > 0)
        Serial.print("FDCAN Sent ID: 0x");
        Serial.println(CAN_tx_msg->id, HEX);
        #endif
    }
}

void CANReceive(CAN_msg_t* CAN_rx_msg) {
    FDCAN_RxHeaderTypeDef rx_header;
    
    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0) {
        if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rx_header, CAN_rx_msg->data) == HAL_OK) {
            CAN_rx_msg->id = rx_header.Identifier;
            CAN_rx_msg->len = rx_header.DataLength >> 16;  // DLC to len
            CAN_rx_msg->format = (rx_header.IdType == FDCAN_STANDARD_ID) ? STANDARD_FORMAT : EXTENDED_FORMAT;
            CAN_rx_msg->type = (rx_header.RxFrameType == FDCAN_REMOTE_FRAME) ? REMOTE_FRAME : DATA_FRAME;
            
            #if (DEBUG_CAN > 0)
            Serial.print("FDCAN Received ID: 0x");
            Serial.println(CAN_rx_msg->id, HEX);
            #endif
        }
    }
}

uint8_t CANMsgAvail(void) {
    return (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0) ? 1 : 0;
}