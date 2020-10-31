/*
 * __user_io_call.c
 *
 *  Created on: Oct 10, 2020
 *      Author: 嵩
 */


#include "main.h"
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_tx;

char tx_buffer[1024];

int _read(int file, char *ptr, int len){
	HAL_StatusTypeDef status;
	char ch;
	status = HAL_UART_Receive(&huart2, &ch, 1, -1);
	ptr[0] = ch;
	return huart2.RxXferSize - huart2.RxXferCount;
}

int _write(int file, char *ptr, int len){
	HAL_StatusTypeDef status;
	while(HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
	for (int i = 0;i < len;i++){
		tx_buffer[i] = ptr[i];
	}
	do{
		status = HAL_UART_Transmit_DMA(&huart2, tx_buffer, len);
		if (status == HAL_ERROR){
			Error_Handler();
		}
	}while(status != HAL_OK);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	if(HAL_UART_DMAStop(&huart2) != HAL_OK){
		Error_Handler();
	}

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_TxCpltCallback can be implemented in the user file.
   */
}
