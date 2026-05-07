#ifndef __PCF8574_LED_H
#define __PCF8574_LED_H

#include "stm32f1xx_hal.h"

#define PCF8574_I2C_ADDR  (0x20 << 1)

void PCF8574_LED_Init(I2C_HandleTypeDef *hi2c);
void PCF8574_LED_Write(uint8_t value);
void PCF8574_LED_Test(void);

#endif
