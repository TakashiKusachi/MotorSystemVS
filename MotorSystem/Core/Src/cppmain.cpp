/*
 * main.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 *
 *  This file define control class and methods with c++.
 *
 */
#ifdef DEBUG
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#else
#ifdef printf
#undef printf
#endif

#define printf(x, ...) dumy_printf(x, ##__VA_ARGS__)

int dumy_printf(const char* format,...){
	return 0;
}
#endif

#include "main.h"
#include "cppmain.hpp"
#include "MotorSystem/MotorSystem.hpp"
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

using nsMotorSystem::nsNode::NodeMotorSystem;
lowMotorSystem lms(10.0/*[ms]*/,256);
NodeMotorSystem ms;

#ifdef DEBUG
namespace debug_functions{
	void logoutput(void);
	void MS_SetModeDUTY(void);
	void SetDuty(float);
	void consoleControl(void);
	bool uartScanf(const char *format,...);
	void set16bitModeFilter_IDorMask(unsigned long* filteridmask, unsigned long stdid,unsigned long exid,unsigned long rtr, unsigned long ide);
	void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig);
	void debug_message_recive(CAN_HandleTypeDef* hcan);
	float GetDuty(void);
	float GetVelocity(void);
	float GetCurrent(void);
	int GetMode(void);
}
#endif


/**
 * main function with c++ source code (class).
 */
void cpp_Init(void){
	#ifdef DEBUG
	HAL_CAN_RegisterCallback(&hcan,HAL_CAN_RX_FIFO1_MSG_PENDING_CB_ID,debug_functions::debug_message_recive);
	#endif

	lms.init(&htim17);
	ms.init(&lms);

	lms.start();
	#ifdef DEBUG
	CAN_FilterTypeDef sFilterConfig;
	printf("DEBUG ActivationNotification: ");
	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK){
		printf("Failure\r\n");
		Error_Handler();
	}
	printf("Done\r\n");

	printf("DEBUG CAN FilterConfigure: ");
	debug_functions::__makeFilterConfig(&sFilterConfig);
	if(HAL_CAN_ConfigFilter(&hcan,&sFilterConfig) != HAL_OK){
		printf("Failure!\r\n");
		Error_Handler();
	}
	printf("Done\r\n");
	#endif
}

void cpp_MainLoop(void){
#ifdef DEBUG
	using namespace debug_functions;

	while(1){
		  consoleControl();
	}
#else
	while(1);
#endif
}

#ifdef DEBUG
namespace debug_functions{

	__IO int mode = 0;
	__IO float velocity=0;
	__IO float duty = 0;
	__IO float current = 0;

	__IO bool recive_begin = false;
	__IO bool recive_get_mode = false;
	__IO bool recive_get_velocity = false;
	__IO bool recive_get_duty = false;
	__IO bool recive_get_current = false;

	//#define LOG_MODEOUTPUT_STRING
	void logoutput(void){
		#ifdef DEBUG
		#ifdef LOG_MODEOUTPUT_STRING
		#define MAX_MODENAME_LENGTH 16
		const char namelist[][MAX_MODENAME_LENGTH] = {
										"NOT_INIT",
										"INITED",
										"READY",
										"DUTY",
										"VELOCITY",
		};
		const char name_notset[] = 		"NOT SET";
		const char* mode_name;

		switch(ms.getMode()){
		case MOTORSYSTEM_STATE::READY:
		case MOTORSYSTEM_STATE::DUTY:
			mode_name =  namelist[ms.getMode()];
			break;
		default:
			mode_name = name_notset;
		}

		printf("mode: [%s], duty: [% 5.4e], speed: [% 5.4e]\r\n",mode_name,ms.getDuty(),ms.getVelocity());
		#else
		printf("mode: [%4d], duty: [% 5.4e], speed: [% 5.4e], current: [% 5.4e]\r\n",GetMode(),GetDuty(),GetVelocity(),GetCurrent());
		#endif
		#endif
	}

	void MS_Begin(void){
		unsigned char data[8];
		recive_begin = false;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::BEGIN,lms.getID()),1,0,data);
		while(!recive_begin);
	}

	void MS_SetModeDUTY(void){
		unsigned char data[8];
		data[0] = (unsigned char)MOTORSYSTEM_STATE::DUTY;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::SET_MODE,lms.getID()),0,1,data);
	}

	void MS_SetModeVelocity(void){
		unsigned char data[8];
		data[0] = (unsigned char)MOTORSYSTEM_STATE::VELOCITY;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::SET_MODE,lms.getID()),0,1,data);
	}

	int GetMode(void){
		unsigned char data[8];
		recive_get_mode = false;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::GET_MODE,lms.getID()), 1, 0, data);
		while(!recive_get_mode);
		return mode;
	}

	void SetDuty(float duty){
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::SET_DUTY,lms.getID()), 0, 4, (uint8_t*)&duty);
	}

	float GetDuty(void){
		unsigned char dumy[8];
		recive_get_duty = false;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::GET_DUTY,lms.getID()), 1, 0, dumy);
		while(!recive_get_duty);
		return duty;
	}

	float GetVelocity(void){
		unsigned char dumy[8];
		recive_get_velocity = false;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::GET_VELOCITY,lms.getID()), 1, 0, dumy);
		while(!recive_get_velocity);
		return velocity;

	}

	float GetCurrent(void){
		unsigned char dumy[8];
		recive_get_current = false;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::GET_CURRENT,lms.getID()), 1, 0, dumy);
		while(!recive_get_current);
		return current;
	}

	void Reset(void){
		unsigned char data[8];
		data[0] = (unsigned char)MOTORSYSTEM_STATE::SYSTEM_RESET;
		lms.sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::SET_MODE,lms.getID()),0,1,data);

	}

	void consoleControl(void){
		int id;
		do{
			printf("1: Info\r\n");
			printf("2: Begin\r\n");
			printf("3: SetMode\r\n");
			printf("4: Duty\r\n");
			printf(">");
			if(uartScanf("%d",&id) == false) continue;
			printf("\r\ninput code: %d\r\n",id);

			switch(id){
			case 1:
				logoutput();
				break;
			case 2:
				MS_Begin();
				break;
			case 3:
				int mode;
				printf("1: Duty\r\n");
				printf("2: Velocity\r\n");
				printf("5: RESET\r\n");
				printf(">");
				if(uartScanf("%d",&mode) == false) continue;
				printf("\r\ninput code: %d\r\n",mode);
				switch(mode){
				case 1:
					MS_SetModeDUTY();
					break;
				case 2:
					MS_SetModeVelocity();
					break;
				case 5:
					Reset();
				}
				break;

			case 4:
				float duty;
				printf("duty >");
				if (uartScanf("%f",&duty) == false){
					printf("not format.\r\n");
					continue;
				}
				SetDuty(duty);
				break;
			default:
				printf("Not Command\r\n");
			}
		}while(1);
	}

	bool uartScanf(const char *format,...){

		bool command_success=false;
		char buffer[256];
		va_list ap;
		do{
			scanf("%256[^\n\r]%*c",buffer);
			va_start(ap,format);
			int ret = vsscanf(buffer,format,ap);
			va_end(ap);

			if ((ret == EOF) || (ret == 0)){
			}else{
				command_success = true;
			}
		}while(!command_success);
		return command_success;
	}

	void set16bitModeFilter_IDorMask(unsigned long* filteridmask, unsigned long stdid,unsigned long exid,unsigned long rtr, unsigned long ide){
		*filteridmask  =  ((stdid & 0x07FF) << 5) | ((rtr & 0x0001) << 4) | ((ide & 0x0001) << 3) | ((exid & 0x00038000) >> 15);
	}

	void __makeFilterConfig(CAN_FilterTypeDef* sFilterConfig){

		sFilterConfig->SlaveStartFilterBank = 0;
		sFilterConfig->FilterActivation = CAN_FILTER_ENABLE;
		sFilterConfig->FilterScale = CAN_FILTERSCALE_16BIT;
		sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
		sFilterConfig->FilterBank = 1;
		sFilterConfig->FilterFIFOAssignment = CAN_FILTER_FIFO1;
		set16bitModeFilter_IDorMask(&sFilterConfig->FilterMaskIdHigh,	(0x01 << 8 | 0x000F),		0x00,0x01,0x00);
		set16bitModeFilter_IDorMask(&sFilterConfig->FilterIdHigh,		(0x00 << 8 | lms.getID()),	0x00,0x01,0x00);
		set16bitModeFilter_IDorMask(&sFilterConfig->FilterMaskIdLow,	(0x01 << 8 | 0x000F),		0x00,0x01,0x00);
		set16bitModeFilter_IDorMask(&sFilterConfig->FilterIdLow,		(0x01 << 8 | lms.getID()),	0x00,0x00,0x00);

	}
	void debug_message_recive(CAN_HandleTypeDef* hcan){
		typedef union{
			struct{
				float data;
				unsigned char _[4];
			}F;
			struct{
				MOTORSYSTEM_STATE mode:8;
				unsigned char _[7];
			}MODE;
			unsigned char data[8];
		}ConverterType;
		ConverterType* converter;

		CAN_RxHeaderTypeDef header;

		uint8_t data[8];

		HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO1,&header,data);
		converter = (ConverterType*)data;

		switch(CAN_MESSAGE_GET_CMD(header.StdId)){
		case GET_MODE:
			mode = converter->MODE.mode;
			recive_get_mode = true;
			break;
		case GET_VELOCITY:
			velocity = converter->F.data;
			recive_get_velocity = true;
			break;
		case GET_DUTY:
			duty = converter->F.data;
			recive_get_duty = true;
			break;
		case GET_CURRENT:
			current = converter->F.data;
			recive_get_current = true;
			break;
		case BEGIN:
			recive_begin= true;
			break;
		}

	}
}
#endif
