/*
 * MotorSystem.cpp
 *
 *  Created on: Nov 2, 2020
 *      Author: 嵩
 */

#include "../MotorSystem/MotorSystem.hpp"

namespace MotorSystem{
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

	MotorSystem::MotorSystem(void){

	}

	void MotorSystem::sendMessage(MOTORSYSTEM_CMD cmd,int rtr,int dlc,unsigned char* data){
		this->low->sendMessage(((cmd << 4) | this->getID()),rtr,dlc,data);
	}

	void MotorSystem::init(lowMotorSystem* lms,int id){
		this->low = lms;
		this->id = id;
	}

	void MotorSystem::begin(){
		unsigned char dumy[1];
		this->recive_begin = false;
		this->sendMessage(MOTORSYSTEM_CMD::BEGIN,1, 0, dumy);
		while(!this->recive_begin);
	}


	void MotorSystem::setMode(MOTORSYSTEM_STATE state){
		ConverterType converter;
		converter.MODE.mode = state;
		this->sendMessage(MOTORSYSTEM_CMD::SET_MODE,0, 1, converter.data);
	}

	MOTORSYSTEM_STATE MotorSystem::getMode(void){
		ConverterType converter;
		this->recive_mode = false;
		this->sendMessage(MOTORSYSTEM_CMD::GET_MODE,1, 0, converter.data);
		while(!this->recive_mode);
		return this->mode;
	}

	void MotorSystem::setDuty(float duty){
		ConverterType converter;
		converter.F.data = duty;
		this->sendMessage(MOTORSYSTEM_CMD::SET_DUTY,0, 4, converter.data);

	}

	float MotorSystem::getDuty(void){
		ConverterType converter;
		this->recive_duty = false;
		this->sendMessage(MOTORSYSTEM_CMD::GET_DUTY,1, 0, converter.data);
		while(!this->recive_duty);
		return this->duty;
	}

	void MotorSystem::setVelocity(float vel){
		ConverterType converter;
		converter.F.data = vel;
		this->sendMessage(MOTORSYSTEM_CMD::SET_VELOCITY,0, 4, converter.data);
	}

	float MotorSystem::getVelocity(void){
		ConverterType converter;
		this->recive_velocity = false;
		this->sendMessage(MOTORSYSTEM_CMD::GET_VELOCITY,1, 0, converter.data);
		while(!this->recive_velocity);
		return this->velocity;
	}

	float MotorSystem::getCurrent(void){
		ConverterType converter;
		this->recive_current = false;
		this->sendMessage(MOTORSYSTEM_CMD::GET_CURRENT,1, 0, converter.data);
		while(!this->recive_current);
		return this->current;
	}

	void MotorSystem::setVoltage(float vol){
		ConverterType converter;
		converter.F.data = vol;
		this->sendMessage(MOTORSYSTEM_CMD::SET_VCC,0, 4, converter.data);
	}

	void MotorSystem::parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data){
		ConverterType* converter;
		converter = (ConverterType*)data;

		if(CAN_MESSAGE_GET_ID(id) != this->getID())return ;

		switch(CAN_MESSAGE_GET_CMD(id)){
		case BEGIN:
			this->recive_begin = true;
			break;
		case GET_MODE:
			this->mode = converter->MODE.mode;
			this->recive_mode = true;
			break;
		case GET_DUTY:
			this->duty = converter->F.data;
			this->recive_duty = true;
			break;
		case GET_VELOCITY:
			this->velocity = converter->F.data;
			this->recive_velocity = true;
			break;
		case GET_CURRENT:
			this->current = converter->F.data;
			this->recive_current = true;
			break;
		}
	}

	void MotorSystem::reset(void){
		this->setMode(MOTORSYSTEM_STATE::SYSTEM_RESET);

	}

	int MotorSystem::getID(void){
		return this->id;
	}
}
