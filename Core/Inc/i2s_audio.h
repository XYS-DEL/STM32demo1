#ifndef __I2S_AUDIO_H
#define __I2S_AUDIO_H

#include "stm32f1xx_hal.h"

void I2S_Audio_Init(void);
void I2S_Audio_PlayTone(uint16_t freq, uint16_t duration_ms);

#endif
