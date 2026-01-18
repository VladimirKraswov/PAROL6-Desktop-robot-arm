#include "adc_init.h"
#include "iodefs.h"

ADC_HandleTypeDef hadc1;

// Инициализация АЦП
void MX_ADC1_Init(void) {
  __HAL_RCC_ADC12_CLK_ENABLE();  // Для H7

  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;  // Адаптировано
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;

  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    // Обработка ошибки
  }

  sConfig.Channel = ADC_CHANNEL_14;  // Адаптировано для PF4 (adc0 - pf4, -, +; в STM32H723 PF4 соответствует ADC_CHANNEL_14 для ADC1/ADC2, проверьте даташит RM0468 для точного маппинга)
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  // Адаптировано для приближения к 15 циклам
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    // Обработка ошибки
  }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle) {
  if(adcHandle->Instance==ADC1) {
    __HAL_RCC_GPIOF_CLK_ENABLE();  // Для PF4 (adc0 - pf4, -, +)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_4;  // PF4 (adc0 - pf4, -, +)
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  }
}

// Чтение АЦП
int ADC_CHANNEL_8_READ_BUS_VOLTAGE(void) {
  HAL_ADC_Start(&hadc1);
  // Poll ADC1 Perihperal & TimeOut = 1mSec
  HAL_ADC_PollForConversion(&hadc1, 1);
  // Read The ADC Conversion Result & Map It To PWM DutyCycle
 int AD_RES = HAL_ADC_GetValue(&hadc1);
 return AD_RES;
}

// Расчет напряжения
int BUS_voltage(void) {
    int R1 = VOLTAGE_DIVIDER_R1;
    int R2 = VOLTAGE_DIVIDER_R2;
    
    float referenceVoltage = ADC_REF_VOLTAGE;
    int adcResolution = (1 << ADC_RESOLUTION) - 1;
    
    int adcValue = ADC_CHANNEL_8_READ_BUS_VOLTAGE();
    
    float voltageDividerRatio = (float)R2 / (R1 + R2);
    
    int measuredVoltage = (int)(adcValue * (referenceVoltage * 1000.0f) / adcResolution / voltageDividerRatio);
    
    return measuredVoltage;
}