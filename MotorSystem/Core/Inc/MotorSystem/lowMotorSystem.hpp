/*
 * lowMotorSystem.hpp
 *
 *  Created on: Oct 29, 2020
 *      Author: 嵩
 */

#ifndef INC_MOTORSYSTEM_LOWMOTORSYSTEM_HPP_
#define INC_MOTORSYSTEM_LOWMOTORSYSTEM_HPP_

#include "MotorSystem/motorSystem.hpp"

/**
 * hardware control class for motor system.
 */
class lowMotorSystem: public MotorSystem::lowMotorSystem{
	float tick_time;
	float countPerRadius;
	float speed;
	int id;
public:
	lowMotorSystem(float tick_time, uint32_t PPR);

	/**
	 *  Initialize funtion.
	 *
	 *  Initalize of the hardware registar and midl
	 *  1. Setting of callback functions.
	 *
	 *  Args:
	 *  	tick_htim (TIM_HandleTypeDef*): TIM Handler of motorsystem control tick generater. use PERIOD_ELAPSED
	 */
	void init(TIM_HandleTypeDef* tick_htim);

	int getID(void) override;

	void begin(void) override;

	void start(void);

	void setDuty(float duty) override;

	void setDirection(int) override;

	void setPPR(int) override;

	float getCurrent(void) override;

	float getSpeed(void) override;

	void sendMessage(uint32_t sid,uint32_t rtr,uint32_t dlc,uint8_t* data) override;

	void NotImplemented(const char*,int) override;

	void ErrorHandler(void) override;

	void controlTick(void);

	void reset(void) override;

};



#endif /* INC_MOTORSYSTEM_LOWMOTORSYSTEM_HPP_ */
