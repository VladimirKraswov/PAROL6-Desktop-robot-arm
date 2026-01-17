#include "Arduino.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "CAN.h"



/**
 * Инициализирует GPIO регистры для CAN.
 *
 * @params: addr    - Указанный адрес GPIO регистра.
 * @params: index   - Указанный индекс GPIO.
 * @params: speed   - Указанное значение регистра OSPEEDR. (Опционально)
 *
 */
void CANSetGpio(GPIO_TypeDef * addr, uint8_t index, uint8_t speed = 3) {
    uint8_t _index2 = index * 2;
    uint8_t _index4 = index * 4;
    uint8_t ofs = 0;
    uint8_t setting;

    if (index > 7) {
      _index4 = (index - 8) * 4;
      ofs = 1;
    }

    uint32_t mask;

    mask = 0xF << _index4;
    addr->AFR[ofs]  &= ~mask;         // Сброс альтернативной функции (Reset alternate function)
    setting = 0x9;                    // AF9
    mask = setting << _index4;
    addr->AFR[ofs]  |= mask;          // Установка альтернативной функции (Set alternate function)

    mask = 0x3 << _index2;
    addr->MODER   &= ~mask;           // Сброс режима (Reset mode)
    setting = 0x2;                    // Режим альтернативной функции (Alternate function mode)
    mask = setting << _index2;
    addr->MODER   |= mask;            // Установка режима (Set mode)

    mask = 0x3 << _index2;
    addr->OSPEEDR &= ~mask;           // Сброс скорости (Reset speed)
    setting = speed;
    mask = setting << _index2;
    addr->OSPEEDR |= mask;            // Установка скорости (Set speed)

    mask = 0x1 << index;
    addr->OTYPER  &= ~mask;           // Сброс типа выхода на двухтактный (Reset Output push-pull)
 
    mask = 0x3 << _index2;
    addr->PUPDR   &= ~mask;           // Сброс подтяжки порта вверх/вниз (Reset port pull-up/pull-down)

}

/**
 * Инициализирует регистры фильтров CAN.
 *
 * bxCAN предоставляет до 28 масштабируемых/настраиваемых банков фильтров идентификаторов для выбора нужных входящих сообщений и отбрасывания остальных.
 *
 * @preconditions   - Этот регистр может быть записан только когда режим инициализации фильтра установлен (FINIT=1) в регистре CAN_FMR.
 * @params: index   - Указанный индекс фильтра. Индексы 27:14 доступны только в устройствах линейки connectivity.
 * @params: scale   - Выбор масштаба фильтра.
 *                    0: Конфигурация с двойной 16-битной шкалой (Dual 16-bit scale configuration)
 *                    1: Конфигурация с одинарной 32-битной шкалой (Single 32-bit scale configuration)
 * @params: mode    - Выбор режима фильтра.
 *                    0: Два 32-битных регистра банка фильтров x находятся в режиме маски идентификатора (Identifier Mask mode)
 *                    1: Два 32-битных регистра банка фильтров x находятся в режиме списка идентификаторов (Identifier List mode)
 * @params: fifo    - Выбор назначенного FIFO для фильтра.
 *                    0: Фильтр назначен на FIFO 0
 *                    1: Фильтр назначен на FIFO 1
 * @params: bank1   - Регистр банка фильтров 1
 * @params: bank2   - Регистр банка фильтров 2
 *
 */
void CANSetFilter(uint8_t index, uint8_t scale, uint8_t mode, uint8_t fifo, uint32_t bank1, uint32_t bank2) {
  if (index > 27) return;

  CAN1->FA1R &= ~(0x1UL<<index);               // Деактивировать фильтр (Deactivate filter)

  if (scale == 0) {
    CAN1->FS1R &= ~(0x1UL<<index);             // Установить фильтр в конфигурацию с двойной 16-битной шкалой (Set filter to Dual 16-bit scale configuration)
  } else {
    CAN1->FS1R |= (0x1UL<<index);              // Установить фильтр в конфигурацию с одинарной 32-битной шкалой (Set filter to single 32 bit configuration)
  }
    if (mode == 0) {
    CAN1->FM1R &= ~(0x1UL<<index);             // Установить фильтр в режим маски (Set filter to Mask mode)
  } else {
    CAN1->FM1R |= (0x1UL<<index);              // Установить фильтр в режим списка (Set filter to List mode)
  }

  if (fifo == 0) {
    CAN1->FFA1R &= ~(0x1UL<<index);            // Назначить фильтр на FIFO 0 (Set filter assigned to FIFO 0)
  } else {
    CAN1->FFA1R |= (0x1UL<<index);             // Назначить фильтр на FIFO 1 (Set filter assigned to FIFO 1)
  }

  CAN1->sFilterRegister[index].FR1 = bank1;    // Установить регистр банка фильтров 1 (Set filter bank registers1)
  CAN1->sFilterRegister[index].FR2 = bank2;    // Установить регистр банка фильтров 2 (Set filter bank registers2)

  CAN1->FA1R |= (0x1UL<<index);                // Активировать фильтр (Activate filter)

}

    
/**
 * Инициализирует контроллер CAN с указанной скоростью передачи данных (битрейтом).
 *
 * @params: bitrate - Указанный битрейт. Если это значение не является одной из определенных констант, скорость будет установлена по умолчанию на 125 Кбит/с.
 * @params: remap   - Выбор порта CAN.
 *                    =0: CAN1_RX сопоставлен с PA11, CAN1_TX сопоставлен с PA12
 *                        CAN2_RX сопоставлен с PB5 , CAN2_TX сопоставлен с PB6
 *                    =1: Не используется
 *                    =2: CAN1_RX сопоставлен с PB8,  CAN1_TX сопоставлен с PB9 (недоступно в 36-выводном корпусе)
 *                        CAN2_RX сопоставлен с PB12, CAN2_TX сопоставлен с PB13
 *                    =3: CAN1_RX сопоставлен с PD0,  CAN1_TX сопоставлен с PD1 (доступно в 100-выводном и 144-выводном корпусах)
 *                        CAN2_RX сопоставлен с PB12, CAN2_TX сопоставлен с PB13
 *
 */
bool CANInit(BITRATE bitrate, int remap)
{
  // Справочное руководство (Reference manual)
  // https://www.st.com/content/ccc/resource/technical/document/reference_manual/4d/ed/bc/89/b5/70/40/dc/DM00135183.pdf/files/DM00135183.pdf/jcr:content/translations/en.DM00135183.pdf
  
  RCC->APB1ENR |= 0x2000000UL;           // Включить тактовый сигнал для CAN1 (Enable CAN1 clock)
  RCC->APB1ENR |= 0x4000000UL;           // Включить тактовый сигнал для CAN2 (Enable CAN2 clock)
  
  if (remap == 0) {
    // CAN1
    RCC->AHB1ENR |= 0x1;                 // Включить тактовый сигнал для GPIOA (Enable GPIOA clock)
    CANSetGpio(GPIOA, 11);               // Настроить PA11 (Set PA11)
    CANSetGpio(GPIOA, 12);               // Настроить PA12 (Set PA12)
    
    // CAN2
    RCC->AHB1ENR |= 0x2;                 // Включить тактовый сигнал для GPIOB (Enable GPIOB clock)
    CANSetGpio(GPIOB, 5);                // Настроить PB5 (Set PB5)
    CANSetGpio(GPIOB, 6);                // Настроить PB6 (Set PB6)
  }

  if (remap == 2) {
    // CAN1
    RCC->AHB1ENR |= 0x2;                 // Включить тактовый сигнал для GPIOB (Enable GPIOB clock)
    CANSetGpio(GPIOB, 8);                // Настроить PB8 (Set PB8)
    CANSetGpio(GPIOB, 9);                // Настроить PB9 (Set PB9)

    // CAN2
    RCC->AHB1ENR |= 0x2;                 // Включить тактовый сигнал для GPIOB (Enable GPIOB clock)
    CANSetGpio(GPIOB, 12);               // Настроить PB12 (Set PB12)
    CANSetGpio(GPIOB, 13);               // Настроить PB13 (Set PB13)
  }
    
  if (remap == 3) {
    // CAN1
    RCC->AHB1ENR |= 0x8;                 // Включить тактовый сигнал для GPIOD (Enable GPIOD clock)
    CANSetGpio(GPIOD, 0);                // Настроить PD0 (Set PD0)
    CANSetGpio(GPIOD, 1);                // Настроить PD1 (Set PD1)

    // CAN2
    RCC->AHB1ENR |= 0x2;                 // Включить тактовый сигнал для GPIOB (Enable GPIOB clock)
    CANSetGpio(GPIOB, 12);               // Настроить PB12 (Set PB12)
    CANSetGpio(GPIOB, 13);               // Настроить PB13 (Set PB13)
  }

  CAN1->MCR |= 0x1UL;                    // Перевести CAN1 в режим инициализации (Require CAN1 to Initialization mode)
  while (!(CAN1->MSR & 0x1UL));          // Ожидание режима инициализации (Wait for Initialization mode)


  CAN2->MCR |= 0x1UL;                    // Перевести CAN2 в режим инициализации (Require CAN2 to Initialization mode)
  while (!(CAN2->MSR & 0x1UL));          // Ожидание режима инициализации (Wait for Initialization mode)


  //CAN1->MCR = 0x51UL;                  // Аппаратная инициализация (Без автоматической повторной передачи) (Hardware initialization(No automatic retransmission))
  CAN1->MCR = 0x41UL;                    // Аппаратная инициализация (С автоматической повторной передачей) (Hardware initialization(With automatic retransmission))

  //CAN2->MCR = 0x51UL;                  // Аппаратная инициализация (Без автоматической повторной передачи) (Hardware initialization(No automatic retransmission))
  CAN2->MCR = 0x41UL;                    // Аппаратная инициализация (С автоматической повторной передачей) (Hardware initialization(With automatic retransmission))

  
  // Установка скорости передачи данных (битрейта) (Set bit rates)
  CAN1->BTR &= ~(((0x03) << 24) | ((0x07) << 20) | ((0x0F) << 16) | (0x3FF));
  CAN1->BTR |=  (((can_configs[bitrate].TS2-1) & 0x07) << 20) | (((can_configs[bitrate].TS1-1) & 0x0F) << 16) | ((can_configs[bitrate].BRP-1) & 0x3FF);

  CAN2->BTR &= ~(((0x03) << 24) | ((0x07) << 20) | ((0x0F) << 16) | (0x3FF));
  CAN2->BTR |=  (((can_configs[bitrate].TS2-1) & 0x07) << 20) | (((can_configs[bitrate].TS1-1) & 0x0F) << 16) | ((can_configs[bitrate].BRP-1) & 0x3FF);


  // Настройка фильтров на значения по умолчанию (Configure Filters to default values)
  CAN1->FMR |=   0x1UL;                  // Установить режим инициализации фильтров (Set to filter initialization mode)
  CAN1->FMR &= 0xFFFFC0FF;               // Очистить начальный банк для CAN2 (Clear CAN2 start bank)

  // bxCAN имеет 28 фильтров.
  // Эти фильтры используются как для CAN1, так и для CAN2.
  // STM32F405 имеет CAN1 и CAN2, поэтому фильтры CAN2 смещены на 14
  CAN1->FMR |= 0xE00;                    // Начальный банк для интерфейса CAN2 (Start bank for the CAN2 interface)

  // Настройка фильтра 0
  // Конфигурация с одинарной 32-битной шкалой (Single 32-bit scale configuration)
  // Два 32-битных регистра банка фильтров x находятся в режиме маски идентификатора (Two 32-bit registers of filter bank x are in Identifier Mask mode)
  // Фильтр назначен на FIFO 0 (Filter assigned to FIFO 0)
  // Регистр банка фильтров установлен на все 0 (Filter bank register to all 0)
  CANSetFilter(0, 1, 0, 0, 0x0UL, 0x0UL);

  // Настройка фильтра 14
  // Конфигурация с одинарной 32-битной шкалой (Single 32-bit scale configuration)
  // Два 32-битных регистра банка фильтров x находятся в режиме маски идентификатора (Two 32-bit registers of filter bank x are in Identifier Mask mode)
  // Фильтр назначен на FIFO 0 (Filter assigned to FIFO 0)
  // Регистр банка фильтров установлен на все 0 (Filter bank register to all 0)
  CANSetFilter(14, 1, 0, 0, 0x0UL, 0x0UL);

  CAN1->FMR &= ~(0x1UL);                 // Деактивировать режим инициализации (Deactivate initialization mode)

  uint16_t TimeoutMilliseconds = 1000;
  bool can2 = false;
  CAN2->MCR &= ~(0x1UL);                 // Перевести CAN2 в нормальный режим (Require CAN2 to normal mode)

  // Ожидание перехода в нормальный режим
  // Если соединение некорректно, он не вернется в нормальный режим.
  for (uint16_t wait_ack = 0; wait_ack < TimeoutMilliseconds; wait_ack++) {
    if ((CAN2->MSR & 0x1UL) == 0) {
      can2 = true;
      break;
    }
    delayMicroseconds(1000);
  }
  //Serial.print("can2=");
  //Serial.println(can2);
  if (can2) {

  } else {

  }

  bool can1 = false;
  CAN1->MCR &= ~(0x1UL);                 // Перевести CAN1 в нормальный режим (Require CAN1 to normal mode)

  // Ожидание перехода в нормальный режим
  // Если соединение некорректно, он не вернется в нормальный режим.
  for (uint16_t wait_ack = 0; wait_ack < TimeoutMilliseconds; wait_ack++) {
    if ((CAN1->MSR & 0x1UL) == 0) {
      can1 = true;
      break;
    }
    delayMicroseconds(1000);
  }
  //Serial.print("can1=");
  //Serial.println(can1);
  if (can1) {

  } else {
    return false;
  }
  return true;
}

 
/**
 * Декодирует CAN сообщения из регистров данных и заполняет
 * структуру сообщения CAN соответствующими полями данных.
 *
 * @preconditions     - Действительное CAN сообщение получено
 * @params CAN_rx_msg - Структура CAN сообщения для приема
 *
 */
void CANReceive(uint8_t ch, CAN_msg_t* CAN_rx_msg)
{
  if(ch == 1) {
    uint32_t id = CAN1->sFIFOMailBox[0].RIR;
    if ((id & STM32_CAN_RIR_IDE) == 0) { // Стандартный формат кадра (Standard frame format)
        CAN_rx_msg->format = STANDARD_FORMAT;;
        CAN_rx_msg->id = (CAN_STD_ID_MASK & (id >> 21U));
    }
    else {                               // Расширенный формат кадра (Extended frame format)
        CAN_rx_msg->format = EXTENDED_FORMAT;;
        CAN_rx_msg->id = (CAN_EXT_ID_MASK & (id >> 3U));
    }

    if ((id & STM32_CAN_RIR_RTR) == 0) {  // Кадр данных (Data frame)
        CAN_rx_msg->type = DATA_FRAME;
    }
    else {                                // Кадр удаленного запроса (Remote frame)
        CAN_rx_msg->type = REMOTE_FRAME;
    }


    CAN_rx_msg->len = (CAN1->sFIFOMailBox[0].RDTR) & 0xFUL;

    CAN_rx_msg->data[0] = 0xFFUL &  CAN1->sFIFOMailBox[0].RDLR;
    CAN_rx_msg->data[1] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDLR >> 8);
    CAN_rx_msg->data[2] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDLR >> 16);
    CAN_rx_msg->data[3] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDLR >> 24);
    CAN_rx_msg->data[4] = 0xFFUL &  CAN1->sFIFOMailBox[0].RDHR;
    CAN_rx_msg->data[5] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDHR >> 8);
    CAN_rx_msg->data[6] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDHR >> 16);
    CAN_rx_msg->data[7] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDHR >> 24);

    CAN1->RF0R |= 0x20UL;
  } // конец CAN1 (end CAN1)

  if(ch == 2) {
    uint32_t id = CAN2->sFIFOMailBox[0].RIR;
    if ((id & STM32_CAN_RIR_IDE) == 0) { // Стандартный формат кадра (Standard frame format)
        CAN_rx_msg->format = STANDARD_FORMAT;;
        CAN_rx_msg->id = (CAN_STD_ID_MASK & (id >> 21U));
    }
    else {                               // Расширенный формат кадра (Extended frame format)
        CAN_rx_msg->format = EXTENDED_FORMAT;;
        CAN_rx_msg->id = (CAN_EXT_ID_MASK & (id >> 3U));
    }

    if ((id & STM32_CAN_RIR_RTR) == 0) {  // Кадр данных (Data frame)
        CAN_rx_msg->type = DATA_FRAME;
    }
    else {                                // Кадр удаленного запроса (Remote frame)
        CAN_rx_msg->type = REMOTE_FRAME;
    }


    CAN_rx_msg->len = (CAN2->sFIFOMailBox[0].RDTR) & 0xFUL;

    CAN_rx_msg->data[0] = 0xFFUL &  CAN2->sFIFOMailBox[0].RDLR;
    CAN_rx_msg->data[1] = 0xFFUL & (CAN2->sFIFOMailBox[0].RDLR >> 8);
    CAN_rx_msg->data[2] = 0xFFUL & (CAN2->sFIFOMailBox[0].RDLR >> 16);
    CAN_rx_msg->data[3] = 0xFFUL & (CAN2->sFIFOMailBox[0].RDLR >> 24);
    CAN_rx_msg->data[4] = 0xFFUL &  CAN2->sFIFOMailBox[0].RDHR;
    CAN_rx_msg->data[5] = 0xFFUL & (CAN2->sFIFOMailBox[0].RDHR >> 8);
    CAN_rx_msg->data[6] = 0xFFUL & (CAN2->sFIFOMailBox[0].RDHR >> 16);
    CAN_rx_msg->data[7] = 0xFFUL & (CAN2->sFIFOMailBox[0].RDHR >> 24);

    CAN2->RF0R |= 0x20UL;
  } // КОНЕЦ CAN2 (END CAN2)

}
 
/**
 * Кодирует CAN сообщения, используя структуру сообщения CAN, и заполняет
 * регистры данных для отправки.
 *
 * @params CAN_tx_msg - Структура CAN сообщения для передачи
 *
 */
void CANSend(uint8_t ch, CAN_msg_t* CAN_tx_msg)
{
  volatile int count = 0;

  uint32_t out = 0;
  if (CAN_tx_msg->format == EXTENDED_FORMAT) { // Расширенный формат кадра (Extended frame format)
      out = ((CAN_tx_msg->id & CAN_EXT_ID_MASK) << 3U) | STM32_CAN_TIR_IDE;
  }
  else {                                       // Стандартный формат кадра (Standard frame format)
      out = ((CAN_tx_msg->id & CAN_STD_ID_MASK) << 21U);
  }

  // Кадр удаленного запроса (Remote frame)
  if (CAN_tx_msg->type == REMOTE_FRAME) {
      out |= STM32_CAN_TIR_RTR;
  }

  if (ch == 1) {
    CAN1->sTxMailBox[0].TDTR &= ~(0xF);
    CAN1->sTxMailBox[0].TDTR |= CAN_tx_msg->len & 0xFUL;

    CAN1->sTxMailBox[0].TDLR  = (((uint32_t) CAN_tx_msg->data[3] << 24) |
                                 ((uint32_t) CAN_tx_msg->data[2] << 16) |
                                 ((uint32_t) CAN_tx_msg->data[1] <<  8) |
                                 ((uint32_t) CAN_tx_msg->data[0]      ));
    CAN1->sTxMailBox[0].TDHR  = (((uint32_t) CAN_tx_msg->data[7] << 24) |
                                 ((uint32_t) CAN_tx_msg->data[6] << 16) |
                                 ((uint32_t) CAN_tx_msg->data[5] <<  8) |
                                 ((uint32_t) CAN_tx_msg->data[4]      ));

    // Команда "Отправить" (Send Go)
    CAN1->sTxMailBox[0].TIR = out | STM32_CAN_TIR_TXRQ;


    /*

    // Ожидание, пока почтовый ящик (mailbox) опустеет
    while(CAN1->sTxMailBox[0].TIR & 0x1UL && count++ < 1000000);

    // Почтовый ящик не становится пустым в цикле ожидания
    if (CAN1->sTxMailBox[0].TIR & 0x1UL) {

    }
    */


  } // конец CAN1 (end CAN1)

  if (ch == 2) {
    CAN2->sTxMailBox[0].TDTR &= ~(0xF);
    CAN2->sTxMailBox[0].TDTR |= CAN_tx_msg->len & 0xFUL;

    CAN2->sTxMailBox[0].TDLR  = (((uint32_t) CAN_tx_msg->data[3] << 24) |
                                 ((uint32_t) CAN_tx_msg->data[2] << 16) |
                                 ((uint32_t) CAN_tx_msg->data[1] <<  8) |
                                 ((uint32_t) CAN_tx_msg->data[0]      ));
    CAN2->sTxMailBox[0].TDHR  = (((uint32_t) CAN_tx_msg->data[7] << 24) |
                                 ((uint32_t) CAN_tx_msg->data[6] << 16) |
                                 ((uint32_t) CAN_tx_msg->data[5] <<  8) |
                                 ((uint32_t) CAN_tx_msg->data[4]      ));

    // Команда "Отправить" (Send Go)
    CAN2->sTxMailBox[0].TIR = out | STM32_CAN_TIR_TXRQ;


    /*
    // Ожидание, пока почтовый ящик (mailbox) опустеет
    while(CAN2->sTxMailBox[0].TIR & 0x1UL && count++ < 1000000);

    // Почтовый ящик не становится пустым в цикле ожидания
    if (CAN2->sTxMailBox[0].TIR & 0x1UL) {
    }
    */


  } // конец CAN2 (end CAN2)

}

 /**
 * Возвращает информацию о наличии доступных CAN сообщений.
 *
 * @returns Наличие ожидающих CAN сообщений в контроллере CAN
 *
 */
uint8_t CANMsgAvail(uint8_t ch)
{
  if (ch == 1) {
    // Проверить наличие ожидающих сообщений в FIFO 0 (Check for pending FIFO 0 messages)
    return CAN1->RF0R & 0x3UL;
  } // конец CAN1 (end CAN1)

  if (ch == 2) {
    // Проверить наличие ожидающих сообщений в FIFO 0 (Check for pending FIFO 0 messages)
    return CAN2->RF0R & 0x3UL;
  } // конец CAN2 (end CAN2)

}