/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "stm32f1xx_hal.h"

#include "HalGpio.h"
#include "HalUart.h"
#include "HalInterrupt.h"

#include "Kernel.h"
#include "MemoryMap.h"
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
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

static void Kernel_init(void);
static void Printf_test(void);

void User_task0(void);
void User_task1(void);
void User_task2(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

static void Printf_test(){
    char* str = "printf pointre test";
    char* nullptr = 0;
    //uint32_t i = 5;

    debug_printf("%s\n\r", "Hello printf");
    debug_printf("output string pointer: %s\n\r", str);
    debug_printf("%s is null pointer, %u number\n\r", nullptr, 10);
    debug_printf("dec = %u hex = %x \n\r", 0xff, 0xff);
    debug_printf("print zero %u\n\r", 0);
}


extern UART_HandleTypeDef huart2;

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
  //Hal_interrupt_init();
  hal_gpio_init();
  hal_uart_init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  int i = 20;
  uint8_t ch_it;
  while (i--)
  {
    /* USER CODE END WHILE */
    uint8_t ch = Hal_uart_get_char();
    Hal_uart_put_char(ch);
    //printf("hajin\n");
    /* USER CODE BEGIN 3 */
  }

  Printf_test();
  debug_printf("\n\rhajin\n\r");
  HAL_UART_Receive_IT(&huart2, &ch_it, 1);
  Kernel_init();
  //uint8_t bef_ch_it;
  //ch_it = 0;
  //bef_ch_it = 0;
  while(1){
  //if(bef_ch_it != ch_it){
	//Hal_uart_put_char(ch_it);
  	//bef_ch_it = ch_it;
  
  //}else{
  //HAL_UART_Receive_IT(&huart2, &ch_it, 1);
	  
  //}//uint32_t j = 0;
  }
  /* USER CODE END 3 */
}

static void Kernel_init(void)
{
    uint32_t taskId;
    Kernel_task_init();
    Kernel_event_flag_init();

    taskId = Kernel_task_create(User_task0);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task0 creation fail\n");
    }
    taskId = Kernel_task_create(User_task1);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task1 creation fail\n");
    }
    taskId = Kernel_task_create(User_task2);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task2 creation fail\n");
    }
    Kernel_start();
}


void User_task0(void)
{
    uint32_t local = 0;
    debug_printf("User Task #0 SP=0x%x\n\r", &local);

    uint8_t  cmdBuf[16];
    uint32_t cmdBufIdx = 0;
    uint8_t  uartch = 0;


    while(true)
    {
        KernelEventFlag_t handle_event = Kernel_wait_events(KernelEventFlag_UartIn|KernelEventFlag_CmdOut);
        switch(handle_event)
        {
        case KernelEventFlag_UartIn:
            Kernel_recv_msg(KernelMsgQ_Task0, &uartch, 1);
	    // debug_printf("recieve msg : %c asd\n\r ", uartch);
            if (uartch == '\n' || uartch == '\r')
            {
		//debug_printf("enter input\n\r");
		
                cmdBuf[cmdBufIdx] = '\0';

                while(true)
                {
                    Kernel_send_events(KernelEventFlag_CmdIn);
                    if (false == Kernel_send_msg(KernelMsgQ_Task1, &cmdBufIdx, 1))
                    {
                        Kernel_yield();
                    }
                    else if (false == Kernel_send_msg(KernelMsgQ_Task1, cmdBuf, cmdBufIdx))
                    {
                        uint8_t rollback;
                        Kernel_recv_msg(KernelMsgQ_Task1, &rollback, 1);
                        Kernel_yield();
                    }
                    else
                    {
                        break;
                    }
                }

                cmdBufIdx = 0;
            }
            else
            {
		//debug_printf("cmdbuf\n\r");
                cmdBuf[cmdBufIdx] = uartch;
                cmdBufIdx++;
                cmdBufIdx %= 16;
            }
	    break;
        case KernelEventFlag_CmdOut:
            debug_printf("CmdOut Event by Task0\n\r");
            break;
        }
        Kernel_yield();
    }
}


void User_task1(void)
{
    uint32_t local = 0;
    debug_printf("User Task #1 SP=0x%x\n\r", &local);

    uint8_t cmdlen = 0;
    uint8_t cmd[16] = {0};

    while(true)
    {

        KernelEventFlag_t handle_event = Kernel_wait_events(KernelEventFlag_CmdIn);
        switch(handle_event)
        {
        case KernelEventFlag_CmdIn:
            memclr(cmd, 16);
            Kernel_recv_msg(KernelMsgQ_Task1, &cmdlen, 1);
            Kernel_recv_msg(KernelMsgQ_Task1, cmd, cmdlen);
            debug_printf("\n\rRecv Cmd: %s\n\r", cmd);
	    break;
        }
        Kernel_yield();
    }
}


void User_task2(void)
{
    uint32_t local = 0;

    debug_printf("User Task #2 SP=0x%x\n\r", &local);

    while(true)
    {
        //debug_printf("User Task #2 SP=0x%x\n\r", &local);
        Kernel_yield();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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


