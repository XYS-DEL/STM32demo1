#include "pcf8574_led.h"

static I2C_HandleTypeDef *hi2c_ext;

void PCF8574_LED_Init(I2C_HandleTypeDef *hi2c) {
    hi2c_ext = hi2c;
    // All LEDs off (pins high = LED off for active-low wiring)
    PCF8574_LED_Write(0xFF);
}

void PCF8574_LED_Write(uint8_t value) {
    // PCF8574: write a byte, each bit controls one pin
    // 0 = pin LOW (LED on), 1 = pin HIGH (LED off) for common active-low LED boards
    uint8_t data = ~value; // invert so 1=on, 0=off from user perspective
    HAL_I2C_Master_Transmit(hi2c_ext, PCF8574_I2C_ADDR, &data, 1, HAL_MAX_DELAY);
}

void PCF8574_LED_Test(void) {
    // Chase pattern: light LEDs one by one
    for (uint8_t i = 0; i < 8; i++) {
        PCF8574_LED_Write(1 << i);
        HAL_Delay(150);
    }
    PCF8574_LED_Write(0x00); // all off
}
