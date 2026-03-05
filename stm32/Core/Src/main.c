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
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ai_platform.h"
#include "cnndqn_data_params.h"
#include "interface.hpp"
#include "cnndqn.h"
#include "Match3Game.h"
#include <stdint.h>
#include "RandomGenerate.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
AI_ALIGNED(32)
static ai_u8 pool1[AI_CNNDQN_DATA_ACTIVATION_1_SIZE];

const ai_handle acts[] = { pool1 };

const ai_handle weights[] = {s_cnndqn_weights_array_u64};

AI_ALIGNED(32)
static ai_float in_data[AI_CNNDQN_IN_1_SIZE];

AI_ALIGNED(32)
static ai_float out_data[AI_CNNDQN_OUT_1_SIZE];

static ai_buffer *ai_input;
static ai_buffer *ai_output;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define InputShape {1,3,4,4}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RNG_HandleTypeDef hrng;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
OLED* oled;

Key* key1;

Key* key2;

Key* key3;

Match3Env* env;

static ai_handle cnndqn = AI_HANDLE_NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_RNG_Init(void);
/* USER CODE BEGIN PFP */
int cnndqn_init(void);
int cnndqn_run(const void *in_data, void *out_data);
void process_data(int8_t (*board)[4] , ai_float *in_data);
uint8_t ai_take_action(const ai_float *out_data);

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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
  
  int8_t board[4][4]={0};
  int8_t mode = 0;
  char info[100];
  int32_t reward = 0;
  bool done = false;
  uint8_t x1=0,y1=0,x2=0,y2=0;
  uint8_t choice1 = 0, choice2 = 0;
  bool your_turn = true;
  int32_t your_score = 0, ai_score = 0;

  key1 = Key_Create(GPIOD, GPIO_PIN_0);
  key2 = Key_Create(GPIOD, GPIO_PIN_1);
  key3 = Key_Create(GPIOD, GPIO_PIN_2);

  oled = OLED_Create(I2C_HARDWARE, &hi2c1);
  OLED_Init(oled);
  OLED_ShowString(oled, 1, 1, "Match3 Game");
  OLED_ShowString(oled, 2, 1, "Mode: ");
  OLED_ShowString(oled, 3, 1, "left:AI Show");
  OLED_ShowString(oled, 4, 1, "right:fight AI");


  while(1){
    if(Key_IsPressed(key1)==true){
        mode = 0;
        break;
    }

    if(Key_IsPressed(key2)==true){
        mode = 1;
        break;
    }
  }

  OLED_Clear(oled);

  env = Match3Env_Create();
  Match3Env_Reset(env);
  
  if (cnndqn_init() != 0) {
    OLED_Clear(oled);
    OLED_ShowString(oled, 1, 1, "AI Init Error");
    while (1);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(mode == 0){
      OLED_ShowString(oled, 1, 12, "AI");
      Match3Env_GetBoard(env,board[0]);
      Match3Game_Render(env, oled, board[0]);
      process_data(board, in_data);
      if (cnndqn_run(in_data, out_data) != 0) {
        OLED_Clear(oled);
        OLED_ShowString(oled, 1, 1, "AI Run Error");
        while (1);
      }
      uint8_t action = ai_take_action(out_data);
      Match3Game_Action_to_XY(action, &y1, &x1, &y2, &x2);
      Match3Game_Swap(oled, y1, x1, y2, x2, board[0]);
      Match3Env_Step(env, action, &reward, &done, info);
      Match3Game_Fresh(oled);
      OLED_ShowSignedNumber(oled, 4, 12, reward,3);
      if(done){
        OLED_Clear(oled);
        OLED_ShowString(oled, 1, 1, info);
        OLED_ShowString(oled, 2, 1, "Reward:");
        OLED_ShowSignedNumber(oled, 2, 8, reward, 3);
        OLED_ShowString(oled, 3, 1, "RST to RESTART");
        Key_Destroy(key1);
        Key_Destroy(key2);
        Key_Destroy(key3);
        Match3Env_Destroy(env);
        while(1);
      }
    }

    if(mode == 1){
      Match3Env_GetBoard(env,board[0]);
      Match3Game_Render(env, oled, board[0]);
      OLED_ShowSignedNumber(oled, 4, 12, your_score,3);

      if(your_turn){
        OLED_ShowString(oled, 1, 12, "YOU");
        OLED_ShowNumber(oled, 3, 9, choice1, 2);
        OLED_ShowNumber(oled, 3, 12, choice2, 2);
        OLED_ShowString(oled, 3, 14, "  ");
        if(Key_IsPressed(key1)==true){
          choice1 += 1;
          if(choice1 > 16){
            choice1 = 0;
          }
        }
        if(Key_IsPressed(key2)==true){
          choice2 += 1;
          if(choice2 > 16){
            choice2 = 0;
          }
        }
        if(Key_IsPressed(key3)==true){
          x1 = (choice1-1) % 4 + 1;
          y1 = (choice1-1) / 4 + 1;
          x2 = (choice2-1) % 4 + 1;
          y2 = (choice2-1) / 4 + 1;
          uint8_t action = Match3Game_XY_to_Action(y1, x1, y2, x2);
          Match3Game_Swap(oled, y1, x1, y2, x2, board[0]);
          Match3Env_Step(env, action, &your_score, &done, info);
          Match3Game_Fresh(oled);
          choice1 = 0;
          choice2 = 0;
          your_turn = false;
        }
      }

      else{
        OLED_ShowString(oled, 1, 12, "AI");
        OLED_ShowString(oled, 3, 9, "AI Turn ");
        process_data(board, in_data);
        if (cnndqn_run(in_data, out_data) != 0) {
          OLED_Clear(oled);
          OLED_ShowString(oled, 1, 1, "AI Run Error");
          while (1);
        }
        uint8_t action = ai_take_action(out_data);
        Match3Game_Action_to_XY(action, &y1, &x1, &y2, &x2);
        Match3Game_Swap(oled, y1, x1, y2, x2, board[0]);
        Match3Env_Step(env, action, &ai_score, &done, info);
        Match3Game_Fresh(oled);
        your_turn = true;
      }

      if(done){
        OLED_Clear(oled);
        OLED_ShowString(oled, 1, 1, info);
        OLED_ShowString(oled, 2, 1, "AIScore:");
        OLED_ShowSignedNumber(oled, 2, 9, ai_score, 3);
        OLED_ShowString(oled, 3, 1, "YourScore:");
        OLED_ShowSignedNumber(oled, 3, 11, your_score, 3);
        OLED_ShowString(oled, 4, 1, "RST to RESTART");
        Key_Destroy(key1);
        Key_Destroy(key2);
        Key_Destroy(key3);
        Match3Env_Destroy(env);
        while(1);
      }

    }
  /* USER CODE END 3 */
 }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int cnndqn_init(void){

  ai_error err;
  err = ai_cnndqn_create_and_init(&cnndqn, acts, weights);
  if (err.type != AI_ERROR_NONE) {
    return -1;
  }

  ai_input = ai_cnndqn_inputs_get(cnndqn, NULL);
  ai_output = ai_cnndqn_outputs_get(cnndqn, NULL);

  if (ai_input == NULL || ai_output == NULL) {
    return -1;
  }

  return 0;

}

int cnndqn_run(const void *in_data, void *out_data){
  ai_i32 batch_size;

  
  ai_input[0].data = AI_HANDLE_PTR(in_data);
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  batch_size = ai_cnndqn_run(cnndqn, &ai_input[0], &ai_output[0]);
  if (batch_size != 1) {
    ai_error err = ai_cnndqn_get_error(cnndqn);
    return -1;
  }
  return 0;
}


void process_data(int8_t (*board)[4] , ai_float *in_data){

  for(uint8_t i = 0; i < 4; i++){
    for(uint8_t j = 0; j < 4; j++){
      if(board[i][j] == 1){
        in_data[i*4+j] = 1;
      }
      else{
        in_data[i*4+j] = 0;
      }
    }
  }

  for(uint8_t i = 0; i < 4; i++){
    for(uint8_t j = 0; j < 4; j++){
      if(board[i][j] == 2){
        in_data[16+i*4+j] = 2;
      }
      else{
        in_data[16+i*4+j] = 0;
      }
    }
  }

  for(uint8_t i = 0; i < 4; i++){
    for(uint8_t j = 0; j < 4; j++){
      if(board[i][j] == 3){
        in_data[32+i*4+j] = 3;
      }
      else{
        in_data[32+i*4+j] = 0;
      }
    }
  }

}


uint8_t ai_take_action(const ai_float *out_data){
  uint8_t max_index = 0;
  for(int i = 1; i < 24; i++){
    if(out_data[i] > out_data[max_index]){
      max_index = i;
    }
  }
  uint8_t num;
  Myrandom_generate(&num, 0, 10);
  
  if(num < 1){ // 10% chance to take a random action
    Myrandom_generate(&max_index, 0, 23);
  }
  return max_index;
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
