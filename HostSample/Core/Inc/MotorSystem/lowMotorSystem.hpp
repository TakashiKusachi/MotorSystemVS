/*
 * lowMotorSystem.hpp
 *
 *  Created on: Nov 2, 2020
 *      Author: 嵩
 */

#ifndef INC_MOTORSYSTEM_LOWMOTORSYSTEM_HPP_
#define INC_MOTORSYSTEM_LOWMOTORSYSTEM_HPP_

#include "MotorSystem/MotorSystem.hpp"
#include "main.h"

#define MAX_NUMBER_OF_MS_INSTANCE 16

class lowMotorSystem:public nsMotorSystem::nsHost::lowMotorSystem{

	int num_instance;
	nsMotorSystem::nsHost::MotorSystem* instances[MAX_NUMBER_OF_MS_INSTANCE];

public:
	lowMotorSystem(void){
		num_instance = 0;
	}

	void init();
	void start(void);
	void sendMessage(unsigned long sid,unsigned long rtr,unsigned long dlc, unsigned char* data) override;
	void setMotorSystemInstance(nsMotorSystem::nsHost::MotorSystem*);

	void ErrorHandler(void) override;

	void __can_recive(CAN_HandleTypeDef* hcan);
};


#endif /* INC_MOTORSYSTEM_LOWMOTORSYSTEM_HPP_ */
