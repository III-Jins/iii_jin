#include "stm32f1xx_hal.h"
#include "HalGpio.h"

void hal_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  /*Configure GPIO pin Output Level */
  /*Configure GPIO pin Output Level */
  /*Configure GPIO pin : B1_Pin */
  /*Configure GPIO pins : F_CS_Pin LCD_RST_Pin LCD_D1_Pin */
  /*Configure GPIO pins : LCD_RD_Pin LCD_WR_Pin LCD_RS_Pin LD2_Pin
                           LCD_D7_Pin LCD_D0_Pin LCD_D2_Pin */
  /*Configure GPIO pins : LCD_CS_Pin LCD_D6_Pin LCD_D3_Pin LCD_D5_Pin
                           LCD_D4_Pin */

  /* EXTI interrupt init*/

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}
