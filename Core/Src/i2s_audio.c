#include "i2s_audio.h"

// Direct register-level I2S driver for STM32F103
// Uses SPI2 peripheral registers directly (I2S is part of SPI2 on STM32F1)

#define I2S_CR2     (*(volatile uint32_t *)(SPI2_BASE + 0x04))
#define I2S_SR      (*(volatile uint32_t *)(SPI2_BASE + 0x08))
#define I2S_DR      (*(volatile uint32_t *)(SPI2_BASE + 0x0C))
#define I2S_I2SCFGR (*(volatile uint32_t *)(SPI2_BASE + 0x1C))
#define I2S_I2SPR   (*(volatile uint32_t *)(SPI2_BASE + 0x20))

// I2SCFGR bits
#define CFG_CHLEN    (1 << 0)
#define CFG_CKPOL    (1 << 3)
#define CFG_I2SCFG_1 (1 << 9)   // Master TX
#define CFG_I2SE     (1 << 10)  // I2S enable
#define CFG_I2SMOD   (1 << 11)  // I2S mode

// I2SPR bits
#define PR_I2SDIV_Pos 0
#define PR_ODD        (1 << 8)

static int16_t audio_buf[128];

void I2S_Audio_Init(void) {
    // Enable GPIOB and SPI2 clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    // PB12 (WS), PB13 (CK), PB15 (SD) -> AF push-pull 50MHz
    GPIOB->CRH = (GPIOB->CRH & ~(0xF << 16)) | (0xB << 16); // PB12
    GPIOB->CRH = (GPIOB->CRH & ~(0xF << 20)) | (0xB << 20); // PB13
    GPIOB->CRH = (GPIOB->CRH & ~(0xF << 28)) | (0xB << 28); // PB15

    // Disable I2S before configuration
    I2S_I2SCFGR = 0;

    // Prescaler: Fs = I2S_CLK / (32 * 2 * (I2SDIV + ODD/2))
    // SYSCLK=64MHz -> I2S_CLK=64MHz
    // I2SDIV=20, ODD=1 -> Fs = 64M / (64 * 20.5) = 48780 Hz
    I2S_I2SPR = (20 << PR_I2SDIV_Pos) | PR_ODD;

    // I2S config: Master TX, Philips, 16-bit
    I2S_I2SCFGR = CFG_I2SMOD | CFG_I2SCFG_1;
}

void I2S_Audio_PlayTone(uint16_t freq, uint16_t duration_ms) {
    uint16_t samples_per_half = 48000 / (freq * 2);
    if (samples_per_half == 0) samples_per_half = 1;

    for (uint16_t i = 0; i < 128; i++) {
        audio_buf[i] = ((i / samples_per_half) % 2) ? 6000 : -6000;
    }

    I2S_I2SCFGR |= CFG_I2SE;

    uint32_t total = (uint32_t)duration_ms * 48;
    uint32_t sent = 0;
    while (sent < total) {
        for (uint16_t i = 0; i < 128 && sent < total; i++) {
            while (!(I2S_SR & SPI_SR_TXE)) {}
            I2S_DR = (uint32_t)(uint16_t)audio_buf[i];
            sent++;
        }
    }

    while (I2S_SR & SPI_SR_BSY) {}

    for (uint8_t i = 0; i < 8; i++) {
        while (!(I2S_SR & SPI_SR_TXE)) {}
        I2S_DR = 0;
    }
    while (I2S_SR & SPI_SR_BSY) {}

    I2S_I2SCFGR &= ~CFG_I2SE;
}
