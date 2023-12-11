#ifndef HAL_HALUART_H_
#define HAL_HALUART_H_

void hal_uart_init(void);
void Hal_uart_put_char(uint8_t ch);
uint8_t Hal_uart_get_char(void);
void Hal_uart_isr(void);

#endif /* HAL_HALUART_H_ */

