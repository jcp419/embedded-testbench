/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "temperature_sens.h"
#include "analog.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define TEMPERATURE_ENABLE
#define ANALOG_ENABLE

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN PV */

#ifdef TEMPERATURE_ENABLE

enum {
	TEMPERATURE_3_3_V_CHANNEL = 0,
	TEMPERATURE_5_V_CHANNEL = 1,
	TEMPERATURE_12_V_CHANNEL = 2,
	TEMPERATURE_DEVICES = 3
};

TemperatureSensor* temperature_channels[TEMPERATURE_DEVICES];

float temperature_data[TEMPERATURE_DEVICES];

SMBus* i2cBus;

#endif

#ifdef ANALOG_ENABLE

 enum {
 	VOLTAGE_DIVIDER_3_3_V = 0,
 	VOLTAGE_DIVIDER_5_V = 1,
 	VOLTAGE_DIVIDER_12_V = 2,
	VOLTAGE_DEVICES = 3
 };

enum {
	CS3_3_3_V = 0,
	CS2_5_V = 1,
	CS1_12_V = 2,
	CURRENT_DEVICES = 3
};

Analog* voltage_channels[VOLTAGE_DEVICES];
Analog* current_channels[CURRENT_DEVICES];

float voltage_data[VOLTAGE_DEVICES];
float current_data[CURRENT_DEVICES];

#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

#ifdef TEMPERATURE_ENABLE

// EFFECTS: sends temperature data in the following format
// $TEMPERATURE,<t0>,<t1>,<t2>
void send_temperature_data();

#endif

#ifdef ANALOG_ENABLE

// EFFECTS: select analog channel
void select_analog_channel(const Analog* analog_device);

// EFFECTS: get analog data in the following format
// FORMAT: $CURRENT,<c0>,<c1>,<c2> and $VOLTAGE,<v0>,<v1>,<v2>
void get_analog_data();

// EFFECTS: send current data in the following format
// FORMAT: $CURRENT,<c0>,<c1>,<c2>
void send_current_data();

// EFFECTS: send voltage data in the following format
// FORMAT: $VOLTAGE,<v0>,<v1>,<v2>
void send_voltage_data();



#endif

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#ifdef TEMPERATURE_ENABLE

// EFFECTS: sends temperature data in the following format
// FORMAT: $TEMPERATURE,<t0>,<t1>,<t2>
void send_temperature_data() {

	// TODO

	// Total length of output of string $TEMPERATURE,x,x,x
	uint8_t buffer[50] = "";

	sprintf((char *)buffer, "$TEMPERATURE,%f,%f,%f\r\n",\
			temperature_data[0], temperature_data[1], temperature_data[2]);

	HAL_I2C_Slave_Seq_Transmit_IT(&hi2c2, buffer, sizeof(buffer), I2C_LAST_FRAME);

}

#endif

#ifdef ANALOG_ENABLE

// EFFECTS: select analog channel
void select_analog_channel(const Analog* analog_device) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, analog_device->select_pins[0]); //S0
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, analog_device->select_pins[1]); //S1
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, analog_device->select_pins[2]); //S2
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, analog_device->select_pins[3]); //S3
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,  1); //ENABLE
}

// EFFECTS: get analog data in the following format
// FORMAT: $CURRENT,<c0>,<c1>,<c2> and $VOLTAGE,<v0>,<v1>,<v2>
void get_analog_data() {

    for (int i = 0; i < CURRENT_DEVICES; ++i) {
    	const Analog* analog_device = current_channels[i];
    	select_analog_channel(analog_device);
    	current_data[i] = get_current_data(analog_device);
    }

    for (int i = 0; i < VOLTAGE_DEVICES; ++i) {
    	const Analog* analog_device = voltage_channels[i];
    	select_analog_channel(analog_device);
    	voltage_data[i] = get_voltage_data(analog_device);
	}
}

// EFFECTS: send current data in the following format
// FORMAT: $CURRENT,<c0>,<c1>,<c2>
void send_current_data() {

	// TODO - verify that we are sending i2c messages properly
	// Total length of output of string $CURRENT,x,x,x
	uint8_t buffer[50] = "";

	sprintf((char *)buffer, "$CURRENT,%f,%f,%f\r\n",\
			current_data[0], current_data[1], current_data[2]);

	HAL_I2C_Slave_Seq_Transmit_IT(&hi2c2, buffer, sizeof(buffer), I2C_LAST_FRAME);
}

// EFFECTS: send analog data in the following format
// FORMAT: $VOLTAGE,<v0>,<v1>,<v2>
void send_voltage_data() {
  
	// TODO - verify that we are sending i2c messages properly
	// Total length of output of string $VOLTAGE,x,x,x
	uint8_t buffer[50] = "";

	sprintf((char *)buffer, "$VOLTAGE,%f,%f,%f\r\n",\
			voltage_data[0], voltage_data[1], voltage_data[2]);

	HAL_I2C_Slave_Seq_Transmit_IT(&hi2c2, buffer, sizeof(buffer), I2C_LAST_FRAME);
}

#endif

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

#ifdef TEMPERATURE_ENABLE

i2cBus = new_smbus(&hi2c1);
disable_DMA(i2cBus);

temperature_channels[TEMPERATURE_3_3_V_CHANNEL] = new_temperature_sensor(i2cBus, 0, 0, 0);
temperature_channels[TEMPERATURE_5_V_CHANNEL] = new_temperature_sensor(i2cBus, 1, 0, 0);
temperature_channels[TEMPERATURE_12_V_CHANNEL] = new_temperature_sensor(i2cBus, 0, 1, 0);

#endif

#ifdef ANALOG_ENABLE

current_channels[CS3_3_3_V] = new_analog(&hadc1, 0, 0, 1, 0);
current_channels[CS2_5_V] = new_analog(&hadc1, 0, 1, 0, 0);
current_channels[CS1_12_V] = new_analog(&hadc1, 0, 0, 0, 0);

voltage_channels[VOLTAGE_DIVIDER_3_3_V] = new_analog(&hadc1, 1, 0, 1, 0);
voltage_channels[VOLTAGE_DIVIDER_5_V] = new_analog(&hadc1, 1, 1, 0, 0);
voltage_channels[VOLTAGE_DIVIDER_12_V] = new_analog(&hadc1, 1, 0, 0, 0);


#endif

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#ifdef TEMPERATURE_ENABLE

      for (int i = 0; i < TEMPERATURE_DEVICES; ++i) {
          temperature_data[i] = get_temperature_data(temperature_channels[i]);
      }

      send_temperature_data(temperature_data);

#endif

#ifdef ANALOG_ENABLE
      get_analog_data();
      send_current_data();
      send_voltage_data();
#endif
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00303D5B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00303D5B;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|Analog_Enable_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Analog_Select_Pin|Analog_SelectA9_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Analog_SelectC6_Pin|Analog_SelectC7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB12 Analog_Enable_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_12|Analog_Enable_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Analog_Select_Pin Analog_SelectA9_Pin */
  GPIO_InitStruct.Pin = Analog_Select_Pin|Analog_SelectA9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Analog_SelectC6_Pin Analog_SelectC7_Pin */
  GPIO_InitStruct.Pin = Analog_SelectC6_Pin|Analog_SelectC7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/