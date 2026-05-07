#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "stm32f1xx_hal.h"

#define BTN_NONE 0
#define BTN_1    1
#define BTN_2    2
#define BTN_3    3
#define BTN_4    4

void    Buttons_Init(void);
uint8_t Buttons_Scan(void);

#endif
