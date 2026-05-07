/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "dht11.h"
#include "i2s_audio.h"
#include "pcf8574_led.h"
#include "buttons.h"
#include "fan_pwm.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static I2C_HandleTypeDef hi2c1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void MX_I2C1_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  MX_I2C1_Init();
  SSD1306_Init(&hi2c1);
  DHT11_Init();
  I2S_Audio_Init();
  PCF8574_LED_Init(&hi2c1);
  Buttons_Init();
  Fan_PWM_Init();

  // ===== TEST 1: Welcome screen =====
  SSD1306_Clear();
  SSD1306_SetCursor(1, 1);
  SSD1306_WriteString("Pet Hospital");
  SSD1306_SetCursor(1, 3);
  SSD1306_WriteString("HW Test Mode");
  SSD1306_SetCursor(1, 5);
  SSD1306_WriteString("Starting...");
  SSD1306_Update();
  HAL_Delay(2000);

  // ===== TEST 2: DHT11 =====
  SSD1306_Clear();
  SSD1306_SetCursor(0, 0);
  SSD1306_WriteString("[2] DHT11 Test");
  SSD1306_Update();
  HAL_Delay(500);

  DHT11_Data dht;
  for (uint8_t attempt = 0; attempt < 3; attempt++) {
      dht = DHT11_Read();
      if (dht.status == 0) break;
      HAL_Delay(2000);
  }
  SSD1306_SetCursor(0, 2);
  if (dht.status == 0) {
      SSD1306_Printf("Temp: %d C", dht.temperature);
      SSD1306_SetCursor(0, 4);
      SSD1306_Printf("Humi: %d %%", dht.humidity);
      SSD1306_SetCursor(0, 6);
      SSD1306_WriteString("Status: OK");
  } else {
      SSD1306_WriteString("DHT11 ERROR!");
      SSD1306_SetCursor(0, 3);
      SSD1306_WriteString("Check wiring");
  }
  SSD1306_Update();
  HAL_Delay(3000);

  // ===== TEST 3: I2S Audio (MAX98357 + Speaker) =====
  SSD1306_Clear();
  SSD1306_SetCursor(0, 0);
  SSD1306_WriteString("[3] I2S Audio");
  SSD1306_SetCursor(0, 2);
  SSD1306_WriteString("Playing tone...");
  SSD1306_Update();
  HAL_Delay(500);

  I2S_Audio_PlayTone(1000, 500);  // 1kHz, 500ms
  HAL_Delay(200);
  I2S_Audio_PlayTone(1500, 500);  // 1.5kHz, 500ms
  HAL_Delay(200);
  I2S_Audio_PlayTone(2000, 500);  // 2kHz, 500ms

  SSD1306_SetCursor(0, 4);
  SSD1306_WriteString("Done!");
  SSD1306_Update();
  HAL_Delay(2000);

  // ===== TEST 4: PCF8574 LED =====
  SSD1306_Clear();
  SSD1306_SetCursor(0, 0);
  SSD1306_WriteString("[4] PCF8574 LED");
  SSD1306_SetCursor(0, 2);
  SSD1306_WriteString("Chasing LEDs...");
  SSD1306_Update();
  HAL_Delay(500);

  PCF8574_LED_Test();

  SSD1306_SetCursor(0, 4);
  SSD1306_WriteString("Done!");
  SSD1306_Update();
  HAL_Delay(2000);

  // ===== TEST 5: Buttons =====
  SSD1306_Clear();
  SSD1306_SetCursor(0, 0);
  SSD1306_WriteString("[5] Button Test");
  SSD1306_SetCursor(0, 2);
  SSD1306_WriteString("Press any key...");
  SSD1306_Update();
  HAL_Delay(500);

  uint8_t btn_pressed = 0;
  while (btn_pressed < 4) {
      uint8_t btn = Buttons_Scan();
      if (btn != BTN_NONE) {
          SSD1306_Clear();
          SSD1306_SetCursor(0, 0);
          SSD1306_WriteString("[5] Button Test");
          SSD1306_SetCursor(0, 2);
          SSD1306_Printf("KEY%d pressed!", btn);
          SSD1306_SetCursor(0, 4);
          SSD1306_Printf("Count: %d/4", btn_pressed + 1);
          SSD1306_SetCursor(0, 6);
          SSD1306_WriteString("Press next key...");
          SSD1306_Update();
          btn_pressed++;
          HAL_Delay(500); // debounce
          // Wait for release
          while (Buttons_Scan() != BTN_NONE) { HAL_Delay(10); }
      }
      HAL_Delay(10);
  }
  HAL_Delay(1000);

  // ===== TEST 6: Buzzer =====
  SSD1306_Clear();
  SSD1306_SetCursor(0, 0);
  SSD1306_WriteString("[6] Buzzer Test");
  SSD1306_SetCursor(0, 2);
  SSD1306_WriteString("Beep! Beep!");
  SSD1306_Update();
  HAL_Delay(500);

  for (uint8_t i = 0; i < 3; i++) {
      HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
      HAL_Delay(200);
      HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
      HAL_Delay(200);
  }
  HAL_Delay(1000);

  // ===== TEST 7: Fan PWM =====
  SSD1306_Clear();
  SSD1306_SetCursor(0, 0);
  SSD1306_WriteString("[7] Fan PWM Test");
  SSD1306_SetCursor(0, 2);
  SSD1306_WriteString("Speed: 0%%");
  SSD1306_Update();
  HAL_Delay(500);

  for (uint16_t spd = 0; spd <= 100; spd += 10) {
      Fan_PWM_SetDuty(spd);
      SSD1306_Clear();
      SSD1306_SetCursor(0, 0);
      SSD1306_WriteString("[7] Fan PWM Test");
      SSD1306_SetCursor(0, 2);
      SSD1306_Printf("Speed: %d%%", spd);
      SSD1306_Update();
      HAL_Delay(400);
  }
  Fan_PWM_Stop();
  SSD1306_SetCursor(0, 4);
  SSD1306_WriteString("Fan stopped");
  SSD1306_Update();
  HAL_Delay(2000);

  // ===== ALL TESTS DONE =====
  SSD1306_Clear();
  SSD1306_SetCursor(1, 2);
  SSD1306_WriteString("All Tests Done!");
  SSD1306_SetCursor(1, 4);
  SSD1306_WriteString("System Ready");
  SSD1306_Update();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(500);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16; // 4MHz * 16 = 64MHz
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // APB1 max 36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void MX_I2C1_Init(void) {
    __HAL_RCC_I2C1_CLK_ENABLE();

    // PB6 = SCL, PB7 = SDA
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = 400000; // 400kHz fast mode
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
