/*
 * main.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#include <stdio.h>
#include "stm32f3xx_hal.h"
#include "MotorSystem/MotorSystem.hpp"

extern "C"{
/*
 * from main.c
 */
	CAN_HandleTypeDef hcan;
	TIM_HandleTypeDef htim1;

	void Error_Handler();
}

class lowMotorSystem: public MotorSystem::lowMotorSystem{
public:
	void setDuty(float duty) override{

	}
	float getCurrent(void) override{

	}
	float getSpeed(void) override{

	}
};

lowMotorSystem lms;

MotorSystem::MotorSystem ms;

extern "C"{
	void cpp_Init(void);
}

void cpp_Init(void){
	ms.init(&lms);
}

