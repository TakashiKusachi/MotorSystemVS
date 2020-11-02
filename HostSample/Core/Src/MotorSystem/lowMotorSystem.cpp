/*
 * lowMotorSystem.cpp
 *
 *  Created on: Nov 2, 2020
 *      Author: 嵩
 */

#include <stdio.h>

#include "main.h"
#include "MotorSystem/lowMotorSystem.hpp"

extern "C"{
	extern CAN_HandleTypeDef hcan;
}


extern lowMotorSystem lms;

extern void __motorsystem_can_recive(CAN_HandleTypeDef* hcan);
extern void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig);

void lowMotorSystem::init(){
	HAL_CAN_RegisterCallback(&hcan,HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID,__motorsystem_can_recive);
}

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

}

void lowMotorSystem::sendMessage(unsigned long sid,unsigned long rtr,unsigned long dlc, unsigned char* data){
	uint32_t transmit_mailbox = 0;
	HAL_StatusTypeDef state;

	CAN_TxHeaderTypeDef header;
	header.StdId = sid;
	header.IDE = CAN_ID_STD;
	header.RTR = rtr == 1?CAN_RTR_REMOTE:CAN_RTR_DATA;
	header.DLC = dlc;
	header.TransmitGlobalTime = DISABLE;

	if(HAL_CAN_AddTxMessage(&hcan,&header,data,&transmit_mailbox) != HAL_OK){
		Error_Handler();
	}
}

void lowMotorSystem::setMotorSystemInstance(MotorSystem::MotorSystem* ms){
	this->instances[this->num_instance] = ms;
	this->num_instance++;
}

void lowMotorSystem::__can_recive(CAN_HandleTypeDef* hcan){
	CAN_RxHeaderTypeDef header;
	uint8_t data[8];
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&header,data);
	for(int i = 0;i < this->num_instance;i++){
		this->instances[i]->parseCANMessage(header.StdId, header.RTR == CAN_RTR_REMOTE, header.DLC, data);
	}

}

void __motorsystem_can_recive(CAN_HandleTypeDef* hcan){
	lms.__can_recive(hcan);
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
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterMaskIdHigh,	(0x01 << 8 | 0x0000),		0x00,0x01,0x00);
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterIdHigh,		(0x00 << 8 | 0x0000),			0x00,0x01,0x00);
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterMaskIdLow,	(0x01 << 8 | 0x0000),		0x00,0x01,0x00);
	set16bitModeFilter_IDorMask(&sFilterConfig->FilterIdLow,		(0x01 << 8 | 0x0000),			0x00,0x00,0x00);

}
