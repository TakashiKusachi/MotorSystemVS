/*
 * MotorSystem.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

//#include <stdio.h>
//#include <math.h>

#include <stddef.h> // NULL
#include "./MotorSystem/MotorSystem.hpp"

#define NOT_IMPLEMENTED_ERROR() this->low->NotImplemented(__FILE__,__LINE__)

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
		this->voltage = 0;
		this->state = NOT_INITIALIZED;
	}

	/**
	 * Initialize
	 */
	returnState MotorSystem::init(lowMotorSystem* low){
		this->low = low;
		this->duty = 0;
		this->setMode(INITIALIZED);
		return RS_OK;
	}

	void MotorSystem::begin(void){
		if(this->state != INITIALIZED)this->low->ErrorHandler();
		this->low->begin();
		this->setMode(READY);
		return;
	}

	/**
	 *
	 */
	void MotorSystem::setMode(MOTORSYSTEM_STATE state){
		MOTORSYSTEM_STATE current = this->state;
		bool illegalStateChange = false;

		if(state == SYSTEM_RESET)this->low->reset();

		switch(current){

		case NOT_INITIALIZED:
			switch(state){
			case INITIALIZED:
				break;
			default:
				illegalStateChange = true;
			}
			break;
		case INITIALIZED:
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
			this->low->ErrorHandler();
		}

		/**
		 * Exit state methods
		 */
		switch(current){
		case NOT_INITIALIZED:
			break;
		case INITIALIZED:
			break;
		case READY:
			break;
		case DUTY:
			break;
		case VELOCITY:
			break;
		}

		this->state = state;

		/**
		 * Join state methods
		 */
		switch(state){
		case NOT_INITIALIZED:
			break;
		case INITIALIZED:
			break;
		case READY:
			this->__setDuty(0.0);
			break;
		case DUTY:
			break;
		case VELOCITY:
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
			this->low->ErrorHandler();
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
			this->low->ErrorHandler();
		}

	}

	float MotorSystem::getVelocity(void){
		if(this->state)
		CHECK_LOWHANDLER(this);
		return this->low->getSpeed();
	}

	float MotorSystem::getCurrent(void){
		CHECK_LOWHANDLER(this);
		if(IS_NOT_BEGIN(this->state))return 0;
		return this->low->getCurrent();
	}

	void MotorSystem::setVoltage(float vol){
		this->supplyVoltage = vol;
	}

	void MotorSystem::controlTick(void){
		CHECK_LOWHANDLER(this);

		/**
		 * If state is VELOCITY mode, it doing PID control.
		 */
		if(this->state == VELOCITY){
			float nowSpeed = this->low->getSpeed();
			float targetSpeed = this->speed;

			float vol = this->speedControler.control(targetSpeed, nowSpeed);
			this->__setVoltage(vol);
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

			case BEGIN:
				this->begin();
				this->low->sendMessage(id,0,0,(unsigned char*)&convert);
				break;

			case GET_MODE:
				convert.MODE.mode = this->getMode();
				this->low->sendMessage(id,0,1,(unsigned char*)&convert);
				break;

			case GET_VELOCITY:
				convert.F.data = this->getVelocity();
				this->low->sendMessage(id, 0, 4, (unsigned char*)&convert);
				break;

			case GET_TORQUE:
				NOT_IMPLEMENTED_ERROR();
				break;

			case GET_DUTY:
				convert.F.data = this->getDuty();
				this->low->sendMessage(id, 0, 4, (unsigned char*)&convert);
				break;
			case GET_CURRENT:
				convert.F.data = this->getCurrent();
				this->low->sendMessage(id, 0, 4, (unsigned char*)&convert);
				break;

			default:
				this->low->ErrorHandler();
				break;
			}
		} else { //if RTR == 0
			switch(GET_CMD(id)){
			case SET_VELOCITY:
				this->setVelocity(pconvert->F.data);
				break;

			case SET_TORQUE:
				NOT_IMPLEMENTED_ERROR();
				break;

			case SET_DUTY:
				this->setDuty(pconvert->F.data);
				break;

			case SET_MODE:
				this->setMode(pconvert->MODE.mode);
				break;
			
			case SET_VCC:
				this->setVoltage(pconvert->F.data);
				break;

			case SET_PPR:
				NOT_IMPLEMENTED_ERROR();
				break;

			case SET_KT:
				NOT_IMPLEMENTED_ERROR();
				break;

			case SET_VGAIN_K:
				this->speedControler.setK(pconvert->F.data);
				break;

			case SET_VGAIN_TI:
				this->speedControler.setTi(pconvert->F.data);
				break;

			case SET_VGAIN_TD:
				this->speedControler.setTd(pconvert->F.data);
				break;

			default:
				this->low->ErrorHandler();
				break;
			}
		}

	}
}
