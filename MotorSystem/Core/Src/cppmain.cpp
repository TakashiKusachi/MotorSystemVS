/*
 * main.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 *
 *  This file define control class and methods with c++.
 *
 */

#include <stdio.h>
#include "main.h"
#include "cppmain.hpp"
#include "stm32f3xx_hal.h"
#include "MotorSystem/MotorSystem.hpp"

extern "C"{
/*
 * external handler from main.c
 */
	CAN_HandleTypeDef hcan;
	TIM_HandleTypeDef htim1;
	TIM_HandleTypeDef htim2;
	TIM_HandleTypeDef htim15;
	TIM_HandleTypeDef htim17;
}

/**
 * Forward declaration
 */
void __motorsystem_tim17_tick(TIM_HandleTypeDef* htim);
void __motorsystem_can_recive(CAN_HandleTypeDef* hcan);
void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig);

void __trigger(TIM_HandleTypeDef* htim);

/**
 * hardware control class for motor system.
 */
class lowMotorSystem: public MotorSystem::lowMotorSystem{
	uint32_t befor_encoder_cont;
	float speed;
public:
	lowMotorSystem(void){
		this->speed = 0;
		this->befor_encoder_cont = 0;
	}

	/**
	 *  Initialize funtion.
	 *
	 *  Initalize of the hardware registar and midl
	 *  1. Setting of callback functions.
	 *
	 *  Args:
	 *  	tick_htim (TIM_HandleTypeDef*): TIM Handler of motorsystem control tick generater. use PERIOD_ELAPSED
	 */
	void init(TIM_HandleTypeDef* tick_htim){

		HAL_TIM_RegisterCallback(tick_htim,HAL_TIM_PERIOD_ELAPSED_CB_ID,__motorsystem_tim17_tick);
		HAL_TIM_RegisterCallback(&htim15,HAL_TIM_TRIGGER_CB_ID,__trigger);
		HAL_TIM_RegisterCallback(&htim15,HAL_TIM_IC_CAPTURE_CB_ID,__trigger);
		HAL_CAN_RegisterCallback(&hcan,HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID,__motorsystem_can_recive);
	}

	/**
	 * Starting modules.
	 */
	void start(){
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

	void setDuty(float duty) override{
		uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,(uint32_t)(period * duty / 100.0));
	}

	float getCurrent(void) override{

	}

	float getSpeed(void) override{
		return this->speed;
	}

	void sendMessage(uint32_t sid,uint32_t rtr,uint32_t dlc,uint8_t* data) override{
		uint32_t transmit_mailbox = 0;

		CAN_TxHeaderTypeDef header;
		header.StdId = sid;
		header.IDE = CAN_ID_STD;
		header.RTR = rtr;
		header.DLC = dlc;
		header.TransmitGlobalTime = DISABLE;

		HAL_CAN_AddTxMessage(&hcan,&header,data,&transmit_mailbox);
	}

	void controlTick(void) {
		//printf("htim2: [%ld]\r\n",htim2.Instance->CNT);
		this->speed = (float)1.0 * (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
		__HAL_TIM_SET_COUNTER(&htim2,0);

	}
};

lowMotorSystem lms;
MotorSystem::MotorSystem ms;

extern "C"{
	void cpp_Init(void);
	void logoutput(void);
	void test_send(float);
}

/**
 * main function with c++ source code (class).
 */
void cpp_Init(void){
	lms.init(&htim17);
	ms.init(&lms);
	lms.start();

	ms.setState(MotorSystem::MOTORSYSTEM_STATE::DUTY);
}

void logoutput(void){
	printf("mode: [%4d], duty: [% 5.4e], speed: [% 5.4e]\r\n",ms.getState(),ms.getDuty(),ms.getSpeed());
}

void test_send(float duty){
	lms.sendMessage(MAKE_CMD(SET_DUTY,0x00), 0, 4, (uint8_t*)&duty);
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

	typedef union{
		struct{
			float data;
			unsigned char _[4];
		}F;
		unsigned char data[8];
	}ConverterType;
	ConverterType convert;

	if (header.RTR == 1){
		switch(GET_CMD(header.StdId)){
		case GET_DUTY:
			convert.F.data = ms.getSpeed();
			lms.sendMessage(header.StdId, 1, 4, (uint8_t*)&convert);
			break;
		default:
			Error_Handler();
			break;
		}
	} else { //if RTR == 0
		switch(GET_CMD(header.StdId)){
		case SET_DUTY:
			ms.setDuty(((ConverterType*)&data)->F.data);
			break;
		default:
			Error_Handler();
			break;
		}
	}


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
