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

lowMotorSystem lms(10.0/*[ms]*/,256);
MotorSystem::MotorSystem ms;

#ifdef DEBUG
namespace debug_functions{
	void logoutput(void);
	void MS_SetModeDUTY(void);
	void SetDuty(float);
	void consoleControl(void);
	bool uartScanf(const char *format,...);
}
#endif


/**
 * main function with c++ source code (class).
 */
void cpp_Init(void){
	lms.init(&htim17);
	ms.init(&lms);
	lms.start();
}

void cpp_MainLoop(void){
#ifdef DEBUG
	using namespace debug_functions;
	int cont = 0;
	int duty = 0;
	const float PI = 3.1415; //
	const int delay_time = 50; // [ms]

	MS_SetModeDUTY();

	while(1){
		  //cont++;
		  //HAL_Delay(delay_time);
		  //HAL_GPIO_TogglePin(GPIOB,GPIO_ON_LED_Pin);

		  //test_send(2*3.141562);
		  //SetDuty(100 * sin((float)cont * (delay_time / 1000.0) * 2.0 * PI));
		  //logoutput();
		  consoleControl();
	}
#else
	while(1);
#endif
}

#ifdef DEBUG
namespace debug_functions{
	//#define LOG_MODEOUTPUT_STRING
	void logoutput(void){
		#ifdef DEBUG
		#ifdef LOG_MODEOUTPUT_STRING
		#define MAX_MODENAME_LENGTH 16
		const char namelist[][MAX_MODENAME_LENGTH] = {
										"NOT_INIT",
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
		printf("mode: [%4d], duty: [% 5.4e], speed: [% 5.4e]\r\n",ms.getMode(),ms.getDuty(),ms.getVelocity());
		#endif
		#endif
	}

	void MS_SetModeDUTY(void){
		unsigned char data[8];
		data[0] = (unsigned char)MOTORSYSTEM_STATE::DUTY;
		lms.sendMessage(MAKE_CMD(MOTORSYSTEM_CMD::SET_MODE,0x00),0,1,data);
	}

	void SetDuty(float duty){
		lms.sendMessage(MAKE_CMD(MOTORSYSTEM_CMD::SET_DUTY,0x00), 0, 4, (uint8_t*)&duty);
	}

	void consoleControl(void){
		int id;
		do{
			printf("1: Info\r\n");
			printf("2: DutyMode\r\n");
			printf(">");
			if(uartScanf("%d",&id) == false) continue;
			printf("input id: %d\r\n",id);

			switch(id){
			case 1:
				logoutput();
				break;
			case 2:
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
}
#endif
