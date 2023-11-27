#include "stdint.h"
#include "stdbool.h"

#include "stm32f1xx_hal.h"

#include "Kernel.h"


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
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

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
	return ch;
}
