/*
 * __user_io_call.c
 *
 *  Created on: Oct 10, 2020
 *      Author: 嵩
 */


#include "stm32f3xx_hal.h"
extern UART_HandleTypeDef huart2;

int __io_putchar(int ch){
	HAL_UART_Transmit(&huart2,(uint8_t*)&ch,1,1);
	return 0;
}
