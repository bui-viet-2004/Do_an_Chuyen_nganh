#include "main.h"
#include "stdio.h"
#include "string.h"
#include "DHT.h"
#include "delay_timer.h"


I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);

#define DHT_GPIO_Port GPIOB
#define DHT_Pin GPIO_PIN_11
#define LED_FAN_PORT   GPIOB
#define LED_FAN_PIN    GPIO_PIN_10
#define LED_AUTO_PORT  GPIOB
#define LED_AUTO_PIN   GPIO_PIN_1
#define Dia_chi_LCD 0x4E
#define UPDATE_INTERVAL 1000 

uint32_t last_update = 0;  

int fan_level = 0;
int fan_power = 0;
int auto_mode = 0;
int fan_level_old = 0;
int fan_power_old = 0;
int auto_mode_old = 0;

char M[100];
char buffer[1];
char buffer_tx[20];

float temperature = 0.0;


void Lcd_Ghi_Lenh (uint8_t lenh)
{
 char data_u, data_l;
 uint8_t data_t[4];
 data_u = (lenh&0xf0);
 data_l = ((lenh<<4)&0xf0);
 data_t[0] = data_u|0x0C; //en=1, rs=0
 data_t[1] = data_u|0x08; //en=0, rs=0
 data_t[2] = data_l|0x0C; //en=1, rs=0
 data_t[3] = data_l|0x08; //en=0, rs=0
 HAL_I2C_Master_Transmit (&hi2c1, Dia_chi_LCD,(uint8_t *) data_t, 4, 100);
} 

void Lcd_Ghi_Dulieu (uint8_t data)
{
 char data_u, data_l;
 uint8_t data_t[4];
 data_u = (data&0xf0);
 data_l = ((data<<4)&0xf0);
 data_t[0] = data_u|0x0D; //en=1, rs=1
 data_t[1] = data_u|0x09; //en=0, rs=1
 data_t[2] = data_l|0x0D; //en=1, rs=1
 data_t[3] = data_l|0x09; //en=0, rs=1
 HAL_I2C_Master_Transmit (&hi2c1, Dia_chi_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_init (void)
{
 Lcd_Ghi_Lenh (0x03);
 HAL_Delay(50);
 Lcd_Ghi_Lenh (0x02);
 HAL_Delay(50);
 Lcd_Ghi_Lenh (0x06);
 HAL_Delay(50);
 Lcd_Ghi_Lenh (0x0c);
 HAL_Delay(50);
 Lcd_Ghi_Lenh (0x28);
 HAL_Delay(50);
 Lcd_Ghi_Lenh (0x80);
} 

void Lcd_Ghi_Chuoi (char *str)
{
 while (*str)
	 Lcd_Ghi_Dulieu (*str++);
}

void Lcd_xoa_manhinh (void)
{
 Lcd_Ghi_Lenh (0x01); //xoa man hinh
	HAL_Delay(2);
}

void Fan_SetLevel (int level)
{
	fan_level = level;
	switch(level)
	{
		case 0: __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); break;
		case 1: __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 333); break;
		case 2: __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 666); break;
		case 3: __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 999); break;
	}
}

void HM10_Process(char cmd)
{
	if(cmd == '0')
	{
		fan_power ^= 1;
		
		if(fan_power == 0)
		{
			Fan_SetLevel(0);
			auto_mode = 0;
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,0);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,0);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,1);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,1);
		}
	}
	else if(cmd == '1' && fan_power) auto_mode ^= 1;
	else if(cmd == '2' && fan_power && auto_mode == 0) Fan_SetLevel(1);
	else if(cmd == '3' && fan_power && auto_mode == 0) Fan_SetLevel(2);
	else if(cmd == '4' && fan_power && auto_mode == 0) Fan_SetLevel(3);
}

void LCD_Update()
{
	Lcd_xoa_manhinh();
	Lcd_Ghi_Lenh(0x80);
	sprintf(M,"Nhiet do: %.1f'C",temperature);
	Lcd_Ghi_Chuoi(M);
	Lcd_Ghi_Lenh(0xC0);
	if(auto_mode) sprintf(M,"Auto:On    Lv: %d",fan_level);
	else sprintf(M,"Auto:Off   Lv: %d",fan_level);
	Lcd_Ghi_Chuoi(M);
	
	
	int len = sprintf(M,
    "#0:%.1f'C\r\n"
    "#1:%s Lv:%d\r\n",
    temperature,
    auto_mode ? "Auto" : "Man",
    fan_level
	);

	HAL_UART_Transmit(&huart1, (uint8_t*)M, len, 100);

	
	if(fan_power != fan_power_old)
	{
    int lenn;

    if(fan_power)
        lenn = sprintf(M, "@Power On\r\n");
    else
        lenn = sprintf(M, "@Power Off\r\n");

    HAL_UART_Transmit(&huart1, (uint8_t*)M, lenn, 100);

    fan_power_old = fan_power;
	}

}

void LED_Update(void)
{
	if(fan_power)
		HAL_GPIO_WritePin(LED_FAN_PORT, LED_FAN_PIN, 1);
	else
		HAL_GPIO_WritePin(LED_FAN_PORT, LED_FAN_PIN, 0);

	if(auto_mode)
		HAL_GPIO_WritePin(LED_AUTO_PORT, LED_AUTO_PIN, 1);
	else
		HAL_GPIO_WritePin(LED_AUTO_PORT, LED_AUTO_PIN, 0);
}	


void EXTI0_IRQHandler(void) 
{ 
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);   
	
	fan_power ^= 1;
	if(fan_power == 0)
	{
		Fan_SetLevel(0);
		auto_mode = 0;
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,0);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,0);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,1);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,1);
	}
		
}

void EXTI1_IRQHandler(void) 
{ 
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
	auto_mode ^= 1;
}

void EXTI2_IRQHandler(void) 
{ 
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2); 
	if(auto_mode == 0 && fan_power) Fan_SetLevel(1);
}

void EXTI3_IRQHandler(void) 
{ 
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
	if(auto_mode == 0 && fan_power) Fan_SetLevel(2);
}

void EXTI4_IRQHandler(void) 
{ 
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4); 
	if(auto_mode == 0 && fan_power) Fan_SetLevel(3);
}


int main(void)
{
  HAL_Init();

  SystemClock_Config();
  
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
	MX_TIM4_Init();
  MX_USART1_UART_Init();

	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_Base_Start(&htim4);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	
	lcd_init();
	
	DHT11_InitTypeDef dht;
	DHT11_StatusTypeDef err;
	HAL_DHT11_Init(&dht, GPIOB, GPIO_PIN_11, &htim4);
	
	Fan_SetLevel(0);
	
	Lcd_xoa_manhinh();
	Lcd_Ghi_Lenh(0x84);
	sprintf(M,"Wait.....");
	Lcd_Ghi_Chuoi(M);
	HAL_Delay(1000);
	LCD_Update();
	LED_Update();
	
  
  while (1)
	{
		uint32_t now = HAL_GetTick();
		if(now - last_update >= UPDATE_INTERVAL)
		{
			last_update = now;
			if(fan_power != fan_power_old || auto_mode != auto_mode_old || fan_level != fan_level_old)
			{
				LCD_Update();
				LED_Update();
				fan_power_old = fan_power;
				auto_mode_old = auto_mode;
				fan_level_old = fan_level;
			}
		}
				
    if(HAL_UART_Receive(&huart1, buffer, 1, 100) == HAL_OK)
    {
			char cmd = buffer[0];
			HM10_Process(cmd);
    }

    
    static uint32_t t = 0;
    if(HAL_GetTick() - t >= 1000)
    {
			t = HAL_GetTick();
			HAL_DHT11_ReadData(&dht);   
			temperature = dht.Temperature;
			if(auto_mode && fan_power)
			{
				if(temperature < 30) Fan_SetLevel(1);
				else if(temperature < 36) Fan_SetLevel(2);
				else Fan_SetLevel(3);
			}
			LCD_Update();
    }
		
		
	}
  
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 71;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65534;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB10 PB11 PB12
                           PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
