#include "buttons.h"

void Buttons_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // KEY1: PB1, KEY2: PB10, KEY3: PB11
    GPIO_InitStruct.Pin   = GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // KEY4: PA3
    GPIO_InitStruct.Pin   = GPIO_PIN_3;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t Buttons_Scan(void) {
    // Returns key number (1-4) if pressed, BTN_NONE (0) if not
    // Active low: pressed = LOW
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)  == GPIO_PIN_RESET) return BTN_1;
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) return BTN_2;
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) return BTN_3;
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)  == GPIO_PIN_RESET) return BTN_4;
    return BTN_NONE;
}
