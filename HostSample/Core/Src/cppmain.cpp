/*
 * cppmain.cpp
 *
 *  Created on: Nov 2, 2020
 *      Author: 嵩
 */

#include <stdio.h>
#include <stdarg.h>
#include "main.h"
#include "cppmain.hpp"

#include "MotorSystem/MotorSystem.hpp"
#include "./MotorSystem/lowMotorSystem.hpp"

using nsMotorSystem::nsHost::MotorSystem;
using nsMotorSystem::MOTORSYSTEM_STATE;

host_lowMotorSystem lms;
MotorSystem ms;

void consoleControl(void);

bool uartScanf(const char*,...);

void cppmain(void){
	int id;
	printf("please enter id:>");
	uartScanf("%d",&id);
	printf("\r\n");
	lms.init();
	ms.init(&lms,id);
	lms.setMotorSystemInstance(&ms);
	lms.start();

	ms.reset();

	while(1){
	  consoleControl();
	}
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


void logoutput(void ){
	printf("mode: [%4d], duty: [% 5.4e], speed: [% 5.4e], current: [% 5.4e]\r\n",ms.getMode(),ms.getDuty(),ms.getVelocity(),ms.getCurrent());
}

void consoleControl(void){
	int id;
	do{
		printf("1: Info\r\n");
		printf("2: Begin\r\n");
		printf("3: SetMode\r\n");
		printf("4: Duty\r\n");
		printf("5: SetPropaties\r\n");
		printf(">");
		if(uartScanf("%d",&id) == false) continue;
		printf("\r\ninput code: %d\r\n",id);

		switch(id){
		case 1:
			logoutput();
			break;
		case 2:
			ms.begin();
			break;
		case 3:
			int mode;
			printf("1: Duty\r\n");
			printf("2: Velocity\r\n");
			printf(">");
			if(uartScanf("%d",&mode) == false) continue;
			printf("\r\ninput code: %d\r\n",mode);
			switch(mode){
			case 1:
				ms.setMode(MOTORSYSTEM_STATE::DUTY);
				break;
			case 2:
				ms.setMode(MOTORSYSTEM_STATE::VELOCITY);
				break;
			}
			break;

		case 4:
			float duty;
			printf("duty >");
			if (uartScanf("%f",&duty) == false){
				printf("not format.\r\n");
				continue;
			}
			ms.setDuty(duty);
			break;
		case 5:
			printf("1: Set Voltage\r\n");
			printf("2: Set PPR\r\n");
			printf("3: Set Kt\r\n");
			printf(">");
			if(uartScanf("%d",&mode) == false) continue;
			printf("\r\ninput code: %d\r\n",mode);
			switch(mode){
			case 1:
				float vol;
				printf("voltage[V]> ");
				if (uartScanf("%f",&vol) == false){
					printf("not format.\r\n");
					continue;
				}
				ms.setVoltage(vol);
				break;
			case 2:
				float PPR;
				printf("PPR[pulse per R]> ");
				if (uartScanf("%f",&PPR) == false){
					printf("not format.\r\n");
					continue;
				}
				ms.setPPR(PPR);
				break;
			case 3:
				float KT;
				printf("Torque constant[mNm/A]> ");
				if (uartScanf("%f",&KT) == false){
					printf("not format.\r\n");
					continue;
				}
				ms.setKT(KT);
				break;
			}
			break;
		default:
			printf("Not Command\r\n");
		}
	}while(1);
}
