/* 
 * ADC для Mellow Fly 3D Super8 (STM32H723)
 * Адаптировано для PlatformIO + Arduino framework
 */

#include "adc_init.h"
#include "iodefs.h"

// Инициализация АЦП для измерения напряжения шины
void MX_ADC1_Init(void) {
    // В Arduino достаточно настроить пин как аналоговый вход
    pinMode(VBUS_PIN, INPUT_ANALOG);
    
    // Установка разрешения АЦП
    analogReadResolution(ADC_RESOLUTION);
}

/**
 * Чтение сырого значения АЦП с канала измерения напряжения шины
 */
int ADC_CHANNEL_8_READ_BUS_VOLTAGE(void) {
    return analogRead(VBUS_PIN);
}

/**
 * Рассчитывает напряжение шины питания постоянного тока
 */
int BUS_voltage(void) {
    int R1 = VOLTAGE_DIVIDER_R1;
    int R2 = VOLTAGE_DIVIDER_R2;
    
    float referenceVoltage = ADC_REF_VOLTAGE;
    int adcResolution = (1 << ADC_RESOLUTION) - 1;
    
    int adcValue = ADC_CHANNEL_8_READ_BUS_VOLTAGE();
    
    // Рассчитать коэффициент делителя напряжения
    float voltageDividerRatio = (float)R2 / (R1 + R2);
    
    // Рассчитать измеренное напряжение в милливольтах
    int measuredVoltage = (int)(adcValue * (referenceVoltage * 1000.0f) / adcResolution / voltageDividerRatio);
    
    return measuredVoltage;
}