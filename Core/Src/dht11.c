#include "dht11.h"

#define DHT11_PORT  GPIOA
#define DHT11_PIN   GPIO_PIN_1

static void delay_us(uint32_t us) {
    // ~4 cycles per iteration at 8MHz (HSI, no PLL)
    volatile uint32_t count = us * 2;
    while (count--) {
        __NOP();
    }
}

static void dht11_set_output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = DHT11_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void dht11_set_input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin  = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static uint8_t dht11_read_bit(void) {
    uint32_t timeout = 0;
    // Wait for high phase
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET) {
        if (++timeout > 200) return 0;
    }
    // Measure high duration
    uint32_t cnt = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        cnt++;
        if (cnt > 200) break;
    }
    return (cnt > 60) ? 1 : 0;
}

void DHT11_Init(void) {
    // Clock already enabled in MX_GPIO_Init
    dht11_set_input();
}

DHT11_Data DHT11_Read(void) {
    DHT11_Data data = {0, 0, 1};
    uint8_t raw[5] = {0};
    uint32_t timeout;

    // ---- Start signal ----
    dht11_set_output();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);  // pull low >= 18ms
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    delay_us(30);   // pull high 20~40us
    dht11_set_input();

    // ---- Wait for DHT11 response (low 80us + high 80us) ----
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        if (++timeout > 500) return data; // timeout
    }
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET) {
        if (++timeout > 500) return data;
    }
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        if (++timeout > 500) return data;
    }

    // ---- Read 40 bits ----
    for (uint8_t i = 0; i < 40; i++) {
        uint8_t bit = dht11_read_bit();
        raw[i / 8] |= (bit << (7 - (i % 8)));
    }

    // ---- Verify checksum ----
    uint8_t checksum = raw[0] + raw[1] + raw[2] + raw[3];
    if (checksum == raw[4]) {
        data.humidity    = raw[0];
        data.temperature = raw[2];
        data.status      = 0; // OK
    }

    return data;
}
