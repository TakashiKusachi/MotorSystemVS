/*
 * lowMotorSystem.cpp
 *
 *  Created on: Oct 29, 2020
 *      Author: 嵩
 */


#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "cppmain.hpp"
#include "MotorSystem/lowMotorSystem.hpp"

extern "C"{
/*
 * external handler from main.c
 */
	extern ADC_HandleTypeDef hadc1;
	extern CAN_HandleTypeDef hcan;
	extern TIM_HandleTypeDef htim1;
	extern TIM_HandleTypeDef htim2;
	extern TIM_HandleTypeDef htim15;
	extern TIM_HandleTypeDef htim17;
}

using nsMotorSystem::nsNode::NodeMotorSystem;
extern lowMotorSystem lms;
extern NodeMotorSystem ms;

/**
 * Forward declaration
 */
extern void __motorsystem_tim17_tick(TIM_HandleTypeDef* htim);
extern void __motorsystem_can_recive(CAN_HandleTypeDef* hcan);
extern void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig);

void __trigger(TIM_HandleTypeDef* htim);

lowMotorSystem::lowMotorSystem(float tick_time, uint32_t PPR){
	this->speed = 0;
	this->tick_time = tick_time;
	this->countPerRadius = 4 * (float)PPR / (2.0 * 3.141592);
}

void lowMotorSystem::init(TIM_HandleTypeDef* tick_htim){
	this->id = (HAL_GPIO_ReadPin(GPIO_SW1_GPIO_Port,GPIO_SW1_Pin) << 3) |
			   (HAL_GPIO_ReadPin(GPIO_SW1_GPIO_Port,GPIO_SW1_Pin) << 2) |
			   (HAL_GPIO_ReadPin(GPIO_SW1_GPIO_Port,GPIO_SW1_Pin) << 1) |
			   (HAL_GPIO_ReadPin(GPIO_SW0_GPIO_Port,GPIO_SW0_Pin) << 0);
	#ifdef DEBUG
	hcan.Init.Mode=CAN_MODE_SILENT_LOOPBACK;
	//if(HAL_CAN_Init(&hcan) != HAL_OK){
	//	Error_Handler();
	//}
	#endif
	HAL_TIM_RegisterCallback(tick_htim,HAL_TIM_PERIOD_ELAPSED_CB_ID,__motorsystem_tim17_tick);
	HAL_TIM_RegisterCallback(&htim15,HAL_TIM_TRIGGER_CB_ID,__trigger);
	HAL_TIM_RegisterCallback(&htim15,HAL_TIM_IC_CAPTURE_CB_ID,__trigger);
	HAL_CAN_RegisterCallback(&hcan,HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID,__motorsystem_can_recive);
}

int lowMotorSystem::getID(void){
	return this->id;
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

void lowMotorSystem::begin(){
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	{
		 /* Calibration Error */
		 Error_Handler();
	}
	if (HAL_ADC_Start(&hadc1) != HAL_OK)
	{
		 /* Start Conversation Error */
		 Error_Handler();
	}
}

void lowMotorSystem::setDuty(float duty){
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,(uint32_t)(period * duty / 100.0));
}

void lowMotorSystem::setDirection(int dir){
	if (dir == 0){
		HAL_GPIO_WritePin(GPIO_DIR_GPIO_Port,GPIO_DIR_Pin,GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(GPIO_DIR_GPIO_Port,GPIO_DIR_Pin,GPIO_PIN_SET);
	}
}

void lowMotorSystem::setPPR(int ppr){
	ms.NotImplemented(__FILE__,__LINE__);
}

float lowMotorSystem::getCurrent(void){
	uint16_t uhADCxConvertedValue = 0;
	if(HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
	 {
		/* End Of Conversion flag not set on time */
		Error_Handler();
	 }
	 else
	 {
		/* ADC conversion completed */
		/*##-5- Get the converted value of regular channel  ######################*/
		uhADCxConvertedValue = HAL_ADC_GetValue(&hadc1);
	 }
	 return uhADCxConvertedValue;
	//this->NotImplemented(__FILE__,__LINE__);
}

float lowMotorSystem::getSpeed(void){
	return this->speed;
}

void lowMotorSystem::sendMessage(uint32_t sid,uint32_t rtr,uint32_t dlc,uint8_t* data){
	uint32_t transmit_mailbox = 0;

	CAN_TxHeaderTypeDef header;
	header.StdId = sid;
	header.IDE = CAN_ID_STD;
	header.RTR = rtr == 1?CAN_RTR_REMOTE:CAN_RTR_DATA;
	header.DLC = dlc;
	header.TransmitGlobalTime = DISABLE;

	HAL_CAN_AddTxMessage(&hcan,&header,data,&transmit_mailbox);
}

void lowMotorSystem::ErrorHandler(void){
	//Error_Handler();
}

void lowMotorSystem::controlTick(void) {
	//printf("htim2: [%ld]\r\n",htim2.Instance->CNT);
	this->speed = (int32_t)__HAL_TIM_GET_COUNTER(&htim2) / (this->countPerRadius * (this->tick_time / 1000.0)) ;
	__HAL_TIM_SET_COUNTER(&htim2,0);
}

void lowMotorSystem::reset(void){
	NVIC_SystemReset();
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
	ms.parseCANMessage(header.StdId, header.RTR == CAN_RTR_REMOTE, header.DLC, data);
	HAL_GPIO_TogglePin(GPIO_ON_LED_GPIO_Port,GPIO_ON_LED_Pin);
}

void set16bitModeFilter_IDorMask(unsigned long* filteridmask, unsigned long stdid,unsigned long exid,unsigned long rtr, unsigned long ide){
	*filteridmask  =  ((stdid & 0x07FF) << 5) | ((rtr & 0x0001) << 4) | ((ide & 0x0001) << 3) | ((exid & 0x00038000) >> 15);
}

void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig){

	sFilterConfig->SlaveStartFilterBank = 0;
	sFilterConfig->FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig->FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig->FilterBank = 0;
	sFilterConfig->FilterFIFOAssignment = CAN_FILTER_FIFO0;
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterMaskIdHigh,	(0x01 << 8 | 0x000F),		0x00,0x01,0x00);
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterIdHigh,		(0x00 << 8 | lms.getID()),	0x00,0x00,0x00);
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterMaskIdLow,	(0x01 << 8 | 0x000F),		0x00,0x01,0x00);
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterIdLow,		(0x01 << 8 | lms.getID()),	0x00,0x01,0x00);

}


void __trigger(TIM_HandleTypeDef* htim){
	//printf("TEST\r\n");
}
