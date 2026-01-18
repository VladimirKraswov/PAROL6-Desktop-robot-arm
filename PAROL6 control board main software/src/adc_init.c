#include "adc_init.h"
#include "iodefs.h"

// Инициализация АЦП
void MX_ADC1_Init(void) {
    pinMode(VBUS_PIN, INPUT_ANALOG);
    analogReadResolution(ADC_RESOLUTION);
}

// Чтение АЦП
int ADC_CHANNEL_8_READ_BUS_VOLTAGE(void) {
    return analogRead(VBUS_PIN);
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