/*
 * MotorSystem.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

//#include <stdio.h>
//#include <math.h>

#include <stddef.h> // NULL
#include "../MotorSystem/MotorSystem.hpp"


#define NOT_IMPLEMENTED_ERROR() this->NotImplemented(__FILE__,__LINE__)
#define ILLIGAL_MODE_CHANGE() this->IlligalModeChange(__FILE__,__LINE__)

/**
 * @namespace
 */
namespace nsMotorSystem
{
namespace nsNode{
	/**
	 * constructor
	 */
	NodeMotorSystem::NodeMotorSystem(void):
			speedControler(	MOTORSYSTEM_DEFAULT_K,
							MOTORSYSTEM_DEFAULT_Ti,
							MOTORSYSTEM_DEFAULT_Td,
							MOTORSYSTEM_DEFAULT_dt){
		this->low = NULL;
		this->voltage = 0;
		this->state = NOT_INITIALIZED;
		this->estate = NOT_ERROR;
	}

	/**
	 * Initialize
	 */
	returnState NodeMotorSystem::init(lowMotorSystem* low){
		this->low = low;
		this->duty = 0;
		this->setMode(INITIALIZED);
		return RS_OK;
	}

	void NodeMotorSystem::begin(void){
		if(this->state != INITIALIZED)this->low->ErrorHandler();
		this->low->begin();
		this->setMode(READY);
		return;
	}

	/**
	 *
	 */
	void NodeMotorSystem::setMode(MOTORSYSTEM_STATE state){
		MOTORSYSTEM_STATE current = this->state;
		bool illegalStateChange = false;

		if(state == SYSTEM_RESET)this->low->reset();
		if(state == ERROR_MODE){
			this->state = state;
			this->__setDuty(0.0);
			return;
		}

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
			ILLIGAL_MODE_CHANGE();
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
			this->speedControler.resetVariable();
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

	MOTORSYSTEM_STATE NodeMotorSystem::getMode(void){
		return this->state;
	}

	returnState NodeMotorSystem::setVoltage(float vol){
		this->supplyVoltage = vol;
	}

	returnState NodeMotorSystem::setPPR(float ppr){
		this->low->setPPR((int)ppr);
	}

	returnState NodeMotorSystem::setKT(float kt){
		NOT_IMPLEMENTED_ERROR();
	}

	returnState NodeMotorSystem::setVGAIN_K(float k){
		this->speedControler.setK(k);
	}

	returnState NodeMotorSystem::setVGAIN_TI(float ti){
		this->speedControler.setTi(ti);
	}

	returnState NodeMotorSystem::setVGAIN_TD(float td){
		this->speedControler.setTd(td);
	}

	returnState NodeMotorSystem::setDuty(float duty){
		if (this->state == DUTY){
			this->__setDuty(duty);
		} else {
			/** Illegal operation*/
			NOT_IMPLEMENTED_ERROR();
		}
	}

	float NodeMotorSystem::getDuty(void){
		CHECK_LOWHANDLER(this);

		return this->duty;
	}

	returnState NodeMotorSystem::setVelocity(float vel){
		if (this->state == VELOCITY){
			this->speed = vel;
		}else {
			/** Illegal operation*/
			NOT_IMPLEMENTED_ERROR();
		}

	}

	float NodeMotorSystem::getVelocity(void){
		if(this->state)
		CHECK_LOWHANDLER(this);
		return this->low->getSpeed();
	}

	returnState NodeMotorSystem::setTorque(float tor){
		NOT_IMPLEMENTED_ERROR();
	}

	float NodeMotorSystem::getTorque(void){
		NOT_IMPLEMENTED_ERROR();
	}

	float NodeMotorSystem::getCurrent(void){
		CHECK_LOWHANDLER(this);
		if(IS_NOT_BEGIN(this->state))return 0;
		return this->low->getCurrent();
	}

	void NodeMotorSystem::controlTick(void){
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

	void NodeMotorSystem::parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data){
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
			switch(CAN_MESSAGE_GET_CMD(id)){

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
				convert.F.data = this->getTorque();
				this->low->sendMessage(id, 0, 4, (unsigned char*)&convert);
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
			switch(CAN_MESSAGE_GET_CMD(id)){
			case SET_VELOCITY:
				this->setVelocity(pconvert->F.data);
				break;

			case SET_TORQUE:
				this->setTorque(pconvert->F.data);
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
				this->setPPR(pconvert->F.data);
				break;

			case SET_KT:
				this->setKT(pconvert->F.data);
				break;

			case SET_VGAIN_K:
				this->setVGAIN_K(pconvert->F.data);
				break;

			case SET_VGAIN_TI:
				this->setVGAIN_TI(pconvert->F.data);
				break;

			case SET_VGAIN_TD:
				this->setVGAIN_TD(pconvert->F.data);
				break;

			default:
				this->low->ErrorHandler();
				break;
			}
		}
	}

	void NodeMotorSystem::sendErrorMessage(void){
		unsigned char data[8];
		data[0] = this->estate;
		this->low->sendMessage(CAN_MESSAGE_MAKE_CMD(MOTORSYSTEM_CMD::SEND_ERROR,this->low->getID()), 0, 1, data);
	}

	void NodeMotorSystem::NotImplemented(const char* filename,long no){
		this->setMode(MOTORSYSTEM_STATE::ERROR_MODE);
		this->estate = NOT_IMPLEMENTED;
		this->sendErrorMessage();
		this->low->ErrorHandler();
	}

	void NodeMotorSystem::IlligalModeChange(const char* filename,long no){
		this->setMode(MOTORSYSTEM_STATE::ERROR_MODE);
		this->estate = ILLIGAL_MODE_CHANGE;
		this->sendErrorMessage();
		this->low->ErrorHandler();
	}
}
}
