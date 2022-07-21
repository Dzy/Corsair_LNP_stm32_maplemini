/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "crc.h"
#include "tim.h"
#include "dma.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
  uint32_t booted;
} __attribute__((aligned(4))) Settings_TypeDef;
extern bool led_trigger;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t device_serial_32[4];
bool ledsupdated = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief Retargets the C library printf function to the USART.
 * @param None
 * @retval None
 */
int _write(int file, char *data, int len) {
   HAL_StatusTypeDef status =
   HAL_UART_Transmit(&huart1, (uint8_t*)data, len, 1000);
   return (status == HAL_OK ? len : 0);
}
/* USER CODE END 0 */
typedef union {
  struct {
    uint8_t b;
    uint8_t r;
    uint8_t g;
  } color;
  uint32_t data;
} PixelRGB_t;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
  HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_2);
  ledsupdated = true;
}

#define NEOPIXEL_ZERO 27
#define NEOPIXEL_ONE  57
#define NEOPIXEL_RESET 0
#define NUM_PIXELS0 6
#define NUM_PIXELS1 6
#define DMA_BUFF_SIZE0 ((NUM_PIXELS0+1)*24)
#define DMA_BUFF_SIZE1 ((NUM_PIXELS1+1)*24)

  PixelRGB_t pixel0[NUM_PIXELS0];
  uint32_t dmaBuffer0[DMA_BUFF_SIZE0];
  uint32_t *pBuff0;
  PixelRGB_t pixel1[NUM_PIXELS1];
  uint32_t dmaBuffer1[DMA_BUFF_SIZE1];
  uint32_t *pBuff1;
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

   int32_t i, j;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  Settings_TypeDef settings;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  MX_CRC_Init();
  MX_USART1_UART_Init();
  printf("%.lx\n",(uint32_t)flash_storage_init(sizeof(settings)));
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t x = 0;
  while (1)
  {
    /* USER CODE END WHILE */
HAL_GPIO_WritePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin, GPIO_PIN_SET);
    /* USER CODE BEGIN 3 */

    for (i = 0; i < NUM_PIXELS0; i++) {
      pixel0[i].color.r = x;
      pixel0[i].color.g = 0;
      pixel0[i].color.b = 0;
    }
    for (i = 0; i < NUM_PIXELS1; i++) {
      pixel1[i].color.r = 0;
      pixel1[i].color.g = 0;
      pixel1[i].color.b = x;
    }
    //printf("colour %d\n", x);

    x+=10;


    pBuff0 = dmaBuffer0;
    for (i = 0; i < NUM_PIXELS0; i++) {
       for (j = 23; j >= 0; j--) {
         if ((pixel0[i].data >> j) & 0x01) {
           *pBuff0 = NEOPIXEL_ONE;
         } else {
           *pBuff0 = NEOPIXEL_ZERO;
         }
         pBuff0++;
     }
    }
    *pBuff0 = NEOPIXEL_RESET;

    pBuff1 = dmaBuffer1;
    for (i = 0; i < NUM_PIXELS1; i++) {
       for (j = 23; j >= 0; j--) {
         if ((pixel1[i].data >> j) & 0x01) {
           *pBuff1 = NEOPIXEL_ONE;
         } else {
           *pBuff1 = NEOPIXEL_ZERO;
         }
         pBuff1++;
     }
    }
    *pBuff1 = NEOPIXEL_RESET;

    //dmaBuffer0[DMA_BUFF_SIZE0 - 1] = 0; // last element must be 0!
    //dmaBuffer1[DMA_BUFF_SIZE1 - 1] = 0; // last element must be 0!
    HAL_Delay(1);
    HAL_GPIO_WritePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin, GPIO_PIN_RESET);
    while(led_trigger != true);

    //printf("HAL_TIM_PWM_Start_DMA\n");
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, dmaBuffer0, DMA_BUFF_SIZE0);
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, dmaBuffer1, DMA_BUFF_SIZE1);
    while(ledsupdated != true);
    //HAL_Delay(1);
    led_trigger = false;

    
    
    //HAL_Delay(10);
    
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
