/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

COM_InitTypeDef BspCOMInit;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

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

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(B1, BUTTON_MODE_EXTI);
  BSP_PB_Init(B2, BUTTON_MODE_EXTI);
  BSP_PB_Init(B3, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */




  //ZMIENNE POMOCNICZE
  uint8_t pulse_value = 0; 		//DO KONTROLOWANIA WARTOŚCI PULSU PWM ( zakres 0 do 255 )
  uint8_t bool_motor_on = 0;	//DO OKREŚLANIA STANU ZASILANIA SILNIKA (0 WYŁĄCZONY, 1 WŁĄCZONY)
  uint8_t bool_direction = 0;	//DO OKREŚLENIA KIERUNKU PRACY SILNIKA (0 L_PWM, 1 R_PWM)

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);	// Funkcja timera 2 powodująca uruchomienie pulsu PWM na kanale 2
  // TIM2CH4 PRZYPISANY JEST DO L_PWM
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); // Funkcja timera 2 powodująca uruchomienie pulsu PWM na kanale 4
  // TIM2CH4 PRZYPISANY JEST DO R_PWM
  __HAL_RCC_GPIOB_CLK_ENABLE(); // WŁĄCZENIE ZEGARA GPIOB - SŁUŻY DO KONTROLI GPIO




  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){ // ODCZYT STANU NA PRZYCISKU BUTTON 1 - KONTROLA WARTOŚCI PWM PODAWANEJ NA STEROWNIK SILNIKA

		HAL_Delay(500); // BUTTON DEBOUNCE

		if( pulse_value < 255 ) pulse_value += 17; // ZWIĘKSZENIE WARTOŚCI PULSU PWM, KAŻDA ZMIANA TO 1/15 MAKSYMALNEGO ZAKRESU (0-255)
		else continue;	// DLA PEŁNEGO ZAKRESU PULSOWANIA NIE JEST WYMAGANA ŻADNA DODATKOWA CZYNNOŚĆ

		for( int i = 0 ; i < pulse_value / 17 ; i++ ){ // PĘTLA SŁUŻĄCA DO ZAŚWIECENIA NIEBIESKĄ DIODĄ LD1 ILOŚĆ RAZY ODPOWIADAJĄCĄ POZIOMOWI WARTOŚCI PULSU PWM
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);	 	// MIGNIĘCIE DIODĄ
			HAL_Delay(100);								// LD1 - DIODA NA PŁYTCE NUCLEO
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);	// KOLOR DIODY NIEBIESKI
			HAL_Delay(100);								//
		}

		if( !bool_direction && bool_motor_on ){ // SPRAWDZENIE TRYBU SILNIKA - KIERUNEK ORAZ WŁ/WYŁ
		// SILNIK WŁĄCZONY, KIERUNEK PRACY NORMALNY
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, pulse_value ); // PODAWANIE NAPIĘCIA NA L_PWM Z ODPOWIEDNIĄ WARTOŚCIĄ PULSU PWM
		} else if ( bool_direction && bool_motor_on ){
		// SILNIK WYŁĄCZONY, PRACA Z NAWROTAMI
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4, pulse_value ); //PODAWANIE NAPIĘCIA NA R_PWM Z ODPOWIEDNIĄ WARTOŚCIĄ PULSU PWM
		}
	}
	if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)){ // ODCZYT STANU NA PRZYCISKU BUTTON 2 - KONTORLA KIERUNKU PRACY SILNIKA

		HAL_Delay(900); // BUTTON DEBOUNCE

		if ( !bool_direction ){ // SPRAWDZENIE TRYBU PRACY SILNIKA - CZY SILNIK JEST W TRYBIE PRACY Z NAWROTAMI
			bool_direction = 1; // ZMIENNA OKREŚLAJĄCA KIERUNEK PRACY SILNIKA USTAWIONA NA 1 - PRACA NAWROTNA
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0); // ZAPALENIE LAMPKI LD2 SYGNALIZUJĄCEJ PRACĘ Z NAWROTAMI
			if( bool_motor_on ){ // STAN PRACY SILNIKA - JEŻELI WŁĄCZONY, NALEŻY ZMIENIĆ KIERUNEK PRACY
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, 0 ); // WSTRZYMANIE PODAWANIA NAPIĘCIA NA L_PWM
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4, pulse_value ); // PODAWANIE NAPIĘCIA NA R_PWM Z ODPOWIEDNIĄ WARTOŚCIĄ PULSU PWM
			}
		} else {
			bool_direction = 0; // ZMIENNA OKREŚLAJĄCA NORMALNY TRYB PRACY SILNIK - BEZ NAWROTÓW
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1); // ZGASZENIE LAMPKI LD2 SYGNALIZUJĄCEJ PRACĘ Z NAWROTAMI
			if( bool_motor_on ){ // STAN PRACY SILNIKA - JEŻELI WŁĄCZONY, NALEŻY ZMIENIĆ KIERUNEK PRACY
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4, 0 ); // WSTRZYMANIE PODAWANIA NAPIĘCIA NA R_PWM
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, pulse_value ); // PODAWANIE NAPIĘCIA NA L_PWM Z ODPOWIEDNIĄ WARTOŚCIĄ PULSU PWM
			}
		}
	}
	if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)){ // ODCZYT STANU NA PRZYCISKU BUTTON 3 - KONTROLA WŁ/WYŁ SILNIKA

		HAL_Delay(900); // BUTTON DEBOUNCE

		if ( !bool_motor_on ){ // SPRAWDZENIE STANU PRACY SILNIKA - CZY SILNIK JEST WŁĄCZONY
			bool_motor_on = 1; // AKTUALIZACJA ZMIENNEJ PRZECHOWUJĄCEJ INFORMACJĘ O DOPROWADZANIU NAPIĘCIU DO SILNIKA - SILNIK WŁĄCZONY
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0); // ZAPALENIE LAMPKI LD3 SYGNALIZUJĄCEJ DOPROWADZANIE NAPIĘCIA DO SILNIKA
			if ( !bool_direction ){ // SPRAWDZENIE TRYBU PRACY SILNIKA - CZY SILNIK JEST W TRYBIE PRACY Z NAWROTAMI
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4, 0 ); // WSTRZYMANIE PODAWANIA NAPIĘCIA NA R_PWM
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, pulse_value ); // PODAWANIE NAPIĘCIA NA L_PWM Z ODPOWIEDNIĄ WARTOŚCIĄ PULSU PWM
			} else {
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, 0 ); // WSTRZYMANIE PODAWANIA NAPIĘCIA NA L_PWM
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4, pulse_value ); // PODAWANIE NAPIĘCIA NA R_PWM Z ODPOWIEDNIĄ WARTOŚCIĄ PULSU PWM
			}
		} else { // SILNIK WYŁĄCZONY
			bool_motor_on = 0; // AKTUALIZACJA ZMIENNEJ PRZECHOWUJĄCEJ INFORMACJĘ O DOPROWADZANIU NAPIĘCIU DO SILNIKA - SILNIK WYŁĄCZONY
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1); // ZGASZENIE LAMPKI LD3 SYGNALIZUJĄCEJ DOPROWADZANIE NAPIĘCIA DO SILNIKA
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, 0 ); // WSTRZYMANIE PODAWANIA NAPIĘCIA NA L_PWM
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4, 0 ); // WSTRZYMANIE PODAWANIA NAPIĘCIA NA R_PWM
		}
	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the SYSCLKSource and SYSCLKDivider
  */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_RC64MPLL_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_WAIT_STATES_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLK_DIV4;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 255;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB2 PB1 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_SWDIO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB14 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  HAL_PWREx_DisableGPIOPullUp(PWR_GPIO_B, PWR_GPIO_BIT_2|PWR_GPIO_BIT_1|PWR_GPIO_BIT_4);

  /**/
  HAL_PWREx_DisableGPIOPullDown(PWR_GPIO_B, PWR_GPIO_BIT_2|PWR_GPIO_BIT_1|PWR_GPIO_BIT_4);

  /**/
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, PWR_GPIO_BIT_2|PWR_GPIO_BIT_0);

  /**/
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B, PWR_GPIO_BIT_14|PWR_GPIO_BIT_5);

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
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // Toggles PB7
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1); // Toggles PB7
	  HAL_Delay(500); // 500ms delay for visible blinking
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
