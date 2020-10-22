/*
 * MotorSystem.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#include <stdio.h>
#include <math.h>
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
		this->setState(READY);
		return RS_OK;
	}

	void MotorSystem::setState(MOTORSYSTEM_STATE state){
		MOTORSYSTEM_STATE current = this->state;
		switch(current){

		case NOT_INITIALIZE:
			switch(state){
			case READY:
				this->state = state;
				break;
			default:
				/* Illegal chenge state
				 *
				 * NOT_INITIALIZE -> Any(not READY)
				 */
				Error_Handler();
			}
			break;

		case READY:
			switch(state){
			default:
				/**
				 * Illegal change state.
				 */
				Error_Handler();
			}
			break;

		default:
			/*Illegal state*/
			Error_Handler();
		}
	}


	returnState MotorSystem::setDuty(float duty){
		CHECK_LOWHANDLER(this);
		this->duty = duty;

		if(this->duty < 0){
			duty = -this->duty;
		}
		this->low->setDuty(duty);
		return RS_OK;
	}

	float MotorSystem::getDuty(void){
		CHECK_LOWHANDLER(this);

		return this->duty;
	}

	void MotorSystem::controlTick(void){
		static int cnt = 0;
		CHECK_LOWHANDLER(this);

		float nowCurrent = this->low->getCurrent();
		float nowSpeed = this->low->getSpeed();
		float targetCurrent = this->current;
		float targetSpeed = this->speed;

		float cduty = this->speedControler.control(targetSpeed, nowSpeed);
		this->setDuty(100.0 * std::sin(cnt++ / 1000.0 * 2.0 * 3.141592));
	}
}
