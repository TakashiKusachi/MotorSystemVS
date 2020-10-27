/*
 * MotorSystem.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#include "main.h"
#include <stdio.h>
#include <math.h>
#include "./MotorSystem/MotorSystem.hpp"

namespace MotorSystem
{
	/**
	 * constructor
	 */
	MotorSystem::MotorSystem(void){
		this->low = NULL;
		this->state = NOT_INITIALIZED;
	}

	/**
	 * Initialize
	 */
	returnState MotorSystem::init(lowMotorSystem* low){
		this->low = low;
		this->duty = 0;
		this->setState(READY);
		return RS_OK;
	}

	/**
	 *
	 */
	void MotorSystem::setState(MOTORSYSTEM_STATE state){
		MOTORSYSTEM_STATE current = this->state;
		bool illegalStateChange = false;

		switch(current){

		case NOT_INITIALIZED:
			switch(state){
			case READY:
				break;
			default:
				illegalStateChange = true;
			}
			break;

		case READY:
			switch(state){
			case DUTY:
				break;
			default:
				illegalStateChange = true;
			}
			break;
		case DUTY:
			break;

		default:
			illegalStateChange = true;
		}

		/**
		 * Illegal state change.
		 */
		if (illegalStateChange){
			Error_Handler();
		}

		/**
		 * Exit state methods
		 */
		switch(current){
		case NOT_INITIALIZED:
			break;
		case READY:
			break;
		case DUTY:
			break;
		}

		this->state = state;

		/**
		 * Join state methods
		 */
		switch(state){
		case READY:
			this->__setDuty(0.0);
			break;
		case DUTY:
			break;
		}
	}

	MOTORSYSTEM_STATE MotorSystem::getState(void){
		return this->state;
	}


	returnState MotorSystem::setDuty(float duty){
		if (this->state == DUTY){
			this->__setDuty(duty);
		} else {
			/** Illegal operation*/
			Error_Handler();
		}
	}

	float MotorSystem::getDuty(void){
		CHECK_LOWHANDLER(this);

		return this->duty;
	}

	float MotorSystem::getSpeed(void){
		CHECK_LOWHANDLER(this);
		return this->low->getSpeed();
	}

	void MotorSystem::controlTick(void){
		static int cnt = 0;
		CHECK_LOWHANDLER(this);

		float nowCurrent = this->low->getCurrent();
		float nowSpeed = this->low->getSpeed();
		float targetCurrent = this->current;
		float targetSpeed = this->speed;

		/**
		 * If state is VELOCITY mode, it doing PID control.
		 */
		if(this->state == VELOCITY){
			float cduty = this->speedControler.control(targetSpeed, nowSpeed);
			this->__setDuty(cduty);
		}
	}
}
