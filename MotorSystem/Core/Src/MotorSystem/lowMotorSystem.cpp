/*
 * lowMotorSystem.cpp
 *
 *  Created on: Oct 29, 2020
 *      Author: 嵩
 */


#include <stdio.h>
#include "main.h"
#include "cppmain.hpp"
#include "MotorSystem/lowMotorSystem.hpp"

extern "C"{
/*
 * external handler from main.c
 */
	extern CAN_HandleTypeDef hcan;
	extern TIM_HandleTypeDef htim1;
	extern TIM_HandleTypeDef htim2;
	extern TIM_HandleTypeDef htim15;
	extern TIM_HandleTypeDef htim17;
}


extern lowMotorSystem lms;
extern MotorSystem::MotorSystem ms;

/**
 * Forward declaration
 */
void __motorsystem_tim17_tick(TIM_HandleTypeDef* htim);
void __motorsystem_can_recive(CAN_HandleTypeDef* hcan);
void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig);

void __trigger(TIM_HandleTypeDef* htim);

lowMotorSystem::lowMotorSystem(float tick_time, uint32_t PPR){
	this->speed = 0;
	this->tick_time = tick_time;
	this->countPerRadius = 4 * (float)PPR / (2.0 * 3.141592);
}


void lowMotorSystem::init(TIM_HandleTypeDef* tick_htim){

	HAL_TIM_RegisterCallback(tick_htim,HAL_TIM_PERIOD_ELAPSED_CB_ID,__motorsystem_tim17_tick);
	HAL_TIM_RegisterCallback(&htim15,HAL_TIM_TRIGGER_CB_ID,__trigger);
	HAL_TIM_RegisterCallback(&htim15,HAL_TIM_IC_CAPTURE_CB_ID,__trigger);
	HAL_CAN_RegisterCallback(&hcan,HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID,__motorsystem_can_recive);
}

	/**
	 * Starting modules.
	 */
void lowMotorSystem::start(){
	CAN_FilterTypeDef sFilterConfig;

	printf("CAN Start: ");
	if(HAL_CAN_Start(&hcan) != HAL_OK){
		printf("Failure\r\n");
		Error_Handler();
	}
	printf("Done\r\n");


	printf("ActivationNotification: ");
	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK){
		printf("Failure\r\n");
		Error_Handler();
	}
	printf("Done\r\n");


	printf("CAN FilterConfigure: ");
	__makeFilterConfig(&sFilterConfig);
	if(HAL_CAN_ConfigFilter(&hcan,&sFilterConfig) != HAL_OK){
		printf("Failure!\r\n");
		Error_Handler();
	}
	printf("Done\r\n");


	printf("TIM1_PWM Mode Start: ");
	if (HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1) != HAL_OK){
		printf("Failure!\r\n");
		Error_Handler();
	}
	printf("Done\r\n");


	printf("TIM2_Encoder Mode Start: ");
	if (HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL) != HAL_OK){
		printf("Failure!\r\n");
		Error_Handler();
	}
	printf("Done\r\n");

	printf("TIM15_Tick Start: ");
	if (HAL_TIM_Base_Start_IT(&htim15) != HAL_OK){
		printf("Failure!\r\n");
		Error_Handler();
	}
	printf("Done\r\n");


	printf("TIM15_IC Start: ");
	if (HAL_TIM_IC_Start_IT(&htim15,TIM_CHANNEL_1) != HAL_OK){
		printf("Failure!\r\n");
		Error_Handler();
	}
	printf("Done\r\n");

	printf("TIM17_Tick Start: ");
	if (HAL_TIM_Base_Start_IT(&htim17) != HAL_OK){
		printf("Failure!\r\n");
		Error_Handler();
	}
	printf("Done\r\n");
}

void lowMotorSystem::setDuty(float duty){
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,(uint32_t)(period * duty / 100.0));
}

float lowMotorSystem::getCurrent(void){

}

float lowMotorSystem::getSpeed(void){
	return this->speed;
}

void lowMotorSystem::sendMessage(uint32_t sid,uint32_t rtr,uint32_t dlc,uint8_t* data){
	uint32_t transmit_mailbox = 0;

	CAN_TxHeaderTypeDef header;
	header.StdId = sid;
	header.IDE = CAN_ID_STD;
	header.RTR = rtr;
	header.DLC = dlc;
	header.TransmitGlobalTime = DISABLE;

	HAL_CAN_AddTxMessage(&hcan,&header,data,&transmit_mailbox);
}

void lowMotorSystem::controlTick(void) {
	//printf("htim2: [%ld]\r\n",htim2.Instance->CNT);
	this->speed = (int32_t)__HAL_TIM_GET_COUNTER(&htim2) / (this->countPerRadius * (this->tick_time / 1000.0)) ;
	__HAL_TIM_SET_COUNTER(&htim2,0);
}

/**
 * callback funtions
 */
void __motorsystem_tim17_tick(TIM_HandleTypeDef* htim){
	//printf("test callback\r\n");
	lms.controlTick();
	ms.controlTick();
}

void __motorsystem_can_recive(CAN_HandleTypeDef* hcan){
	CAN_RxHeaderTypeDef header;
	uint8_t data[8];
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&header,data);
	ms.parseCANMessage(header.StdId, header.RTR == 1, header.DLC, data);

	HAL_GPIO_TogglePin(GPIO_DIR_GPIO_Port,GPIO_DIR_Pin);
}

void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig){
	sFilterConfig->SlaveStartFilterBank = 0;
	sFilterConfig->FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig->FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig->FilterBank = 0;
	sFilterConfig->FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig->FilterMaskIdHigh = 0x00;
	sFilterConfig->FilterMaskIdLow = 0x00;
	sFilterConfig->FilterIdHigh = 0x00;

}


void __trigger(TIM_HandleTypeDef* htim){
	//printf("TEST\r\n");
}
