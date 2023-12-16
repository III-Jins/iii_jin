#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

#include "stm32f1xx_hal.h"

#include "Kernel.h"

#define HAL_MAX_DELAY      0xFFFFFFFFU

UART_HandleTypeDef huart2;


void hal_uart_init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.gState = HAL_UART_STATE_RESET;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);
  /* USER CODE BEGIN USART2_Init 2 */
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE END USART2_Init 2 */

}

void Hal_uart_put_char(uint8_t ch)
{
	HAL_UART_Transmit(&huart2, &ch, 1, 1000);
}


uint8_t Hal_uart_get_char(void)
{
	uint8_t ch;
	HAL_UART_Receive(&huart2, &ch, 1, 1000);
	//HAL_UART_Receive_IT(&huart2, &ch, 1);
	return ch;
}

void Hal_uart_isr(uint8_t ch_it)
{
	//debug_printf("Interrupt Cause\n\r");
	//uint8_t isr_ch;
	//HAL_UART_Receive(&huart2, &isr_ch, 1,1000);
	//debug_printf("Input value : %d\n\r", isr_ch);

	if (ch_it == '\r' || ch_it == '\n')
	{
		Hal_uart_put_char('\n');
		debug_printf("Enter input \n\r");

		ch_it = '\0';
		//Kernel_send_msg(KernelMsgQ_DebugCmd, &ch, 1);
	    	Kernel_send_events(KernelEventFlag_CmdIn);
	}
	else if (ch_it == 'x')
	{
	    	Kernel_send_events(KernelEventFlag_CmdOut);
	}
	else
	{
		Hal_uart_put_char(ch_it);
		//debug_printf("Normal input \n\r");
	    	Kernel_send_events(KernelEventFlag_UartIn);
	    	//Kernel_send_msg(KernelMsgQ_DebugCmd, &ch, 1);
	}
}
