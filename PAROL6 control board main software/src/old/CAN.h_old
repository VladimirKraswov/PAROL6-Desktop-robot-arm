#ifndef CAN_UTILS_H
#define CAN_UTILS_H

#define DEBUG 0

/* Символические имена для скорости передачи данных (битрейта) CAN сообщения (Symbolic names for bit rate of CAN message) */
typedef enum {CAN_50KBPS, CAN_100KBPS, CAN_125KBPS, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS} BITRATE;

/* Символические имена для форматов CAN сообщения (Symbolic names for formats of CAN message) */
typedef enum {STANDARD_FORMAT = 0, EXTENDED_FORMAT} CAN_FORMAT;

/* Символические имена для типа CAN сообщения (Symbolic names for type of CAN message) */
typedef enum {DATA_FRAME = 0, REMOTE_FRAME}         CAN_FRAME;

/* Структура для представления CAN сообщения (Structure to represent a CAN message) */
typedef struct
{
  uint32_t id;        /* 29-битный идентификатор (29 bit identifier) */
  uint8_t  data[8];   /* Поле данных (Data field) */
  uint8_t  len;       /* Длина поля данных в байтах (Length of data field in bytes) */
  uint8_t  ch;        /* Канал объекта (Не используется) (Object channel(Not use)) */
  uint8_t  format;    /* 0 - СТАНДАРТНЫЙ, 1 - РАСШИРЕННЫЙ ИДЕНТИФИКАТОР (0 - STANDARD, 1- EXTENDED IDENTIFIER) */
  uint8_t  type;      /* 0 - КАДР ДАННЫХ, 1 - КАДР УДАЛЕННОГО ЗАПРОСА (0 - DATA FRAME, 1 - REMOTE FRAME) */
} CAN_msg_t;

/* Структура для конфигурации синхронизации битов CAN (Structure for CAN bit timing configuration) */
typedef const struct
{
  uint8_t TS2; /* Время выборки 2 (Time Sample 2) */
  uint8_t TS1; /* Время выборки 1 (Time Sample 1) */
  uint8_t BRP; /* Скорость передачи в шине (Baud Rate Prescaler) */
} CAN_bit_timing_config_t;

/* Предустановленные конфигурации синхронизации для различных битрейтов (Predefined timing configurations for various bitrates) */
CAN_bit_timing_config_t can_configs[6] = {{2, 12, 60}, {2, 12, 30}, {2, 12, 24}, {2, 12, 12}, {2, 12, 6}, {1, 7, 5}};

/* Битовые маски для регистров CAN (Bit masks for CAN registers) */
#define STM32_CAN_TIR_TXRQ              (1U << 0U)  // Бит 0: Запрос на передачу почтового ящика (Transmit Mailbox Request)
#define STM32_CAN_RIR_RTR               (1U << 1U)  // Бит 1: Запрос удаленной передачи (Remote Transmission Request)
#define STM32_CAN_RIR_IDE               (1U << 2U)  // Бит 2: Расширение идентификатора (Identifier Extension)
#define STM32_CAN_TIR_RTR               (1U << 1U)  // Бит 1: Запрос удаленной передачи (Remote Transmission Request)
#define STM32_CAN_TIR_IDE               (1U << 2U)  // Бит 2: Расширение идентификатора (Identifier Extension)

/* Маски для фильтрации идентификаторов (Masks for identifier filtering) */
#define CAN_EXT_ID_MASK                 0x1FFFFFFFU /* Маска для расширенного (29-битного) идентификатора (Mask for extended (29-bit) identifier) */
#define CAN_STD_ID_MASK                 0x000007FFU /* Маска для стандартного (11-битного) идентификатора (Mask for standard (11-bit) identifier) */

/* Прототипы функций (Function prototypes) */

/* Вычисляет параметры синхронизации CAN на основе тактовой частоты и целевого битрейта (Computes CAN timing parameters based on clock rate and target bitrate) */
int16_t ComputeCANTimings(const uint32_t peripheral_clock_rate,
                          const uint32_t target_bitrate,
                          CAN_bit_timing_config_t* const out_timings);

/* Инициализирует GPIO для CAN (Initializes GPIO for CAN) */
void CANSetGpio(GPIO_TypeDef * addr, uint8_t index, uint8_t speed);

/* Настраивает фильтр CAN (Configures a CAN filter) */
void CANSetFilter(uint8_t index, uint8_t scale, uint8_t mode, uint8_t fifo, uint32_t bank1, uint32_t bank2);

/* Инициализирует контроллер CAN с заданным битрейтом и переназначением выводов (Initializes the CAN controller with given bitrate and pin remapping) */
bool CANInit(BITRATE bitrate, int remap);

/* Принимает CAN сообщение из указанного канала (Receives a CAN message from the specified channel) */
void CANReceive(uint8_t ch, CAN_msg_t* CAN_rx_msg);

/* Отправляет CAN сообщение через указанный канал (Sends a CAN message through the specified channel) */
void CANSend(uint8_t ch, CAN_msg_t* CAN_tx_msg);

/* Проверяет наличие доступных CAN сообщений в указанном канале (Checks for available CAN messages in the specified channel) */
uint8_t CANMsgAvail(uint8_t ch);


#endif  // CAN_UTILS_H