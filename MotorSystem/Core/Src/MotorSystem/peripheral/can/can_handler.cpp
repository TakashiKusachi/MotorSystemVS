/*
 * can_handler.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#include <stdio.h>
#include "stm32f3xx_hal.h"

extern "C" void Error_Handler(void);

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
}

