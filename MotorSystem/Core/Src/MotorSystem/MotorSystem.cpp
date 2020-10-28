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
	MotorSystem::MotorSystem(void):
			speedControler(	MOTORSYSTEM_DEFAULT_K,
							MOTORSYSTEM_DEFAULT_Ti,
							MOTORSYSTEM_DEFAULT_Td,
							MOTORSYSTEM_DEFAULT_dt){
		this->low = NULL;
		this->state = NOT_INITIALIZED;
	}

	/**
	 * Initialize
	 */
	returnState MotorSystem::init(lowMotorSystem* low){
		this->low = low;
		this->duty = 0;
		this->setMode(READY);
		return RS_OK;
	}

	void MotorSystem::begin(void){
		return;
	}

	/**
	 *
	 */
	void MotorSystem::setMode(MOTORSYSTEM_STATE state){
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
			case VELOCITY:
				break;
			default:
				illegalStateChange = true;
			}
			break;
		case DUTY:
			break;
		case VELOCITY:
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

	MOTORSYSTEM_STATE MotorSystem::getMode(void){
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

	returnState MotorSystem::setVelocity(float vel){
		if (this->state == VELOCITY){
			this->speed = vel;
		}else {
			/** Illegal operation*/
			Error_Handler();
		}

	}

	float MotorSystem::getVelocity(void){
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

	void MotorSystem::parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data){
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
		ConverterType convert;
		ConverterType* pconvert = (ConverterType*)data;

		if (rtr == true){
			switch(GET_CMD(id)){
			case GET_DUTY:
				convert.F.data = this->getVelocity();
				this->low->sendMessage(id, 1, 4, (uint8_t*)&convert);
				break;
			default:
				Error_Handler();
				break;
			}
		} else { //if RTR == 0
			switch(GET_CMD(id)){
			case SET_DUTY:
				this->setDuty(pconvert->F.data);
				break;
			case SET_VELOCITY:
				this->setVelocity(pconvert->F.data);
				break;

			case SET_MODE:
				this->setMode(pconvert->MODE.mode);
				break;

			default:
				Error_Handler();
				break;
			}
		}

	}
}
