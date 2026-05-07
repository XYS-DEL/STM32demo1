#ifndef __SSD1306_H
#define __SSD1306_H

#include "stm32f1xx_hal.h"

#define SSD1306_I2C_ADDR  (0x3C << 1)
#define SSD1306_WIDTH     128
#define SSD1306_HEIGHT    64

void SSD1306_Init(I2C_HandleTypeDef *hi2c);
void SSD1306_Clear(void);
void SSD1306_Update(void);
void SSD1306_SetCursor(uint8_t x, uint8_t y);
void SSD1306_WriteChar(char ch);
void SSD1306_WriteString(const char *str);
void SSD1306_Printf(const char *fmt, ...);

#endif
