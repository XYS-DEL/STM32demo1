#ifndef __FAN_PWM_H
#define __FAN_PWM_H

#include "stm32f1xx_hal.h"

void Fan_PWM_Init(void);
void Fan_PWM_SetDuty(uint16_t duty);
void Fan_PWM_Stop(void);

#endif
