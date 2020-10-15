/*
 * MotorSystem.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#include <stdio.h>
#include "./MotorSystem/MotorSystem.hpp"

extern "C"{
	void Error_Handler(void);
}

namespace MotorSystem
{

	MotorSystem::MotorSystem(void){
		this->low = NULL;
		this->state = NOT_INITIALIZE;
	}

	returnState MotorSystem::init(lowMotorSystem* low){
		this->low = low;
		this->duty = 0;
		this->state = READY;
		return RS_OK;
	}

	returnState MotorSystem::setDuty(float duty){
		CHECK_LOWHANDLER(this);

		this->duty = duty;
		this->low->setDuty(duty);
		return RS_OK;
	}

	float MotorSystem::getDuty(void){
		CHECK_LOWHANDLER(this);

		return this->duty;
	}

	void MotorSystem::controlTick(void){
		CHECK_LOWHANDLER(this);

		float nowCurrent = this->low->getCurrent();
		float nowSpeed = this->low->getSpeed();
		float targetCurrent = this->current;
		float targetSpeed = this->speed;

		float cduty = this->speedControler.control(targetSpeed, nowSpeed);
		this->setDuty(cduty);
	}
}
