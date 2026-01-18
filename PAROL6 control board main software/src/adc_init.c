/* ПОЛЬЗОВАТЕЛЬСКИЙ КОД: НАЧАЛО ЗАГОЛОВКА */
/**
  ******************************************************************************
  * @file    adc.c (аналого-цифровой преобразователь)
  * @brief   Этот файл содержит код для настройки
  *          экземпляров АЦП.
  ******************************************************************************
  * @attention (ВНИМАНИЕ)
  *
  * Copyright (c) 2023 STMicroelectronics.
  * Все права защищены.
  *
  * Данное программное обеспечение лицензировано на условиях, которые можно найти в файле LICENSE
  * в корневом каталоге этого программного компонента.
  * Если файл LICENSE не поставляется с этим программным обеспечением, оно предоставляется "КАК ЕСТЬ".
  *
  ******************************************************************************
  */
/* ПОЛЬЗОВАТЕЛЬСКИЙ КОД: КОНЕЦ ЗАГОЛОВКА */
/* Подключаемые файлы ------------------------------------------------------------------*/

#include "adc_init.h"

ADC_HandleTypeDef hadc1;

/* Функция инициализации АЦП1 */
void MX_ADC1_Init(void)
{

  /* Включить тактовый сигнал для периферии АЦП1 */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /* Настройка делителя тактовой частоты АЦП */
  /* Внимание: На STM32F1, максимальная частота тактирования АЦП составляет 14МГц (обратитесь к */
  /*          техническому описанию устройства).                                                */
  /*          Следовательно, делитель тактовой частоты АЦП должен быть настроен в зависимости  */
  /*          от частоты источника тактирования, чтобы оставаться ниже этой максимальной       */
  /*          частоты.                                                                          */

  /*
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  */
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Общая конфигурация
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 3;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

  HAL_ADC_Init(&hadc1);


  /** Настроить обычный канал
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES; ///ADC_SAMPLETIME_7CYCLES_5
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

}

void HAL_ADC_MspInit_(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* ПОЛЬЗОВАТЕЛЬСКИЙ КОД: НАЧАЛО ADC1_MspInit 0 */

  /* ПОЛЬЗОВАТЕЛЬСКИЙ КОД: КОНЕЦ ADC1_MspInit 0 */
    /* Включение тактирования АЦП1 */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**Конфигурация GPIO для АЦП1
    PB0     ------> ADC1_IN8 (Вход АЦП1, канал 8)
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* ПОЛЬЗОВАТЕЛЬСКИЙ КОД: НАЧАЛО ADC1_MspInit 1 */

  /* ПОЛЬЗОВАТЕЛЬСКИЙ КОД: КОНЕЦ ADC1_MspInit 1 */
  }
}

/**
 * Измеряет напряжение шины питания постоянного тока.
 * Возвращает напряжение в милливольтах
 * @param[out] v_
*/
int ADC_CHANNEL_8_READ_BUS_VOLTAGE(void){

  HAL_ADC_Start(&hadc1);
  // Опрос периферии АЦП1 с таймаутом = 1 мс
  HAL_ADC_PollForConversion(&hadc1, 1);
  // Чтение результата преобразования АЦП и преобразование его в коэффициент заполнения ШИМ
 int AD_RES = HAL_ADC_GetValue(&hadc1);
 return AD_RES;

}

/**
 * Рассчитывает напряжение нашей шины питания постоянного тока.
 * Возвращает напряжение в мВ
 * Используются резисторы номиналом 110кОм и 16кОм
 * @param[out] v_
*/
int BUS_voltage(void){

int R1 = 110000; // 110к Ом
int R2 = 16000;  // 16к Ом


float referenceVoltage = 3.3; // 3.3В
int adcResolution = 4095;      // 12-битный АЦП, поэтому 2^12 - 1

int adcValue  = ADC_CHANNEL_8_READ_BUS_VOLTAGE();

// Рассчитать коэффициент делителя напряжения
float voltageDividerRatio = (float)R2 / (R1 + R2); // Приведение к float для деления с плавающей точкой

// Рассчитать измеренное напряжение в милливольтах
int measuredVoltage = (int)(adcValue * (referenceVoltage * 1000.0f) / adcResolution / voltageDividerRatio);

return measuredVoltage;
}