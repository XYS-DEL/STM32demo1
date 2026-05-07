#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f1xx_hal.h"

typedef struct {
    uint8_t  temperature;
    uint8_t  humidity;
    uint8_t  status;  // 0=OK, 1=error
} DHT11_Data;

void DHT11_Init(void);
DHT11_Data DHT11_Read(void);

#endif
