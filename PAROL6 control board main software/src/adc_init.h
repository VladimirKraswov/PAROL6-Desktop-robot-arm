#ifndef __ADC_H__
#define __ADC_H__

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADC_RESOLUTION
#define ADC_RESOLUTION 12
#endif

#define ADC_REF_VOLTAGE 3.3f
#define VBUS_PIN PF4  // adc0 - pf4, -, + (remap для избежания конфликта с heat0 - pb0)

#define VOLTAGE_DIVIDER_R1 110000
#define VOLTAGE_DIVIDER_R2 16000

void MX_ADC1_Init(void);
int ADC_CHANNEL_8_READ_BUS_VOLTAGE(void);
int BUS_voltage(void);

#ifdef __cplusplus
}
#endif

#endif

extern ADC_HandleTypeDef hadc1;