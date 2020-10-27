/*
 * MotorSystem.hpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#ifndef INC_MOTORSYSTEM_MOTORSYSTEM_HPP_
#define INC_MOTORSYSTEM_MOTORSYSTEM_HPP_

#include "main.h"
#include "motorSystem_util.hpp"
#include "pid/PID.hpp"

namespace MotorSystem{

	/**
	 * Interface of peripheral for MotorSystem.
	 */
	class lowMotorSystem{
	public:
		/**
		 * set Duty [%].
		 *
		 * Args:
		 * 		duty (float): duty
		 */
		virtual void setDuty(float) = 0;

		/**
		 * get current [A].
		 *
		 *  Returns:
		 *  	(float): current [A].
		 */
		virtual float getCurrent(void) = 0;

		/**
		 * get speed [rad. / s].
		 */
		virtual float getSpeed(void) = 0;

		/**
		 *
		 */
		virtual void sendMessage(uint32_t sid,uint32_t rtr,uint32_t dlc,uint8_t* data) = 0;
	};

	typedef enum{
		NOT_INITIALIZED,
		READY,
		DUTY,
		VELOCITY,
	}MOTORSYSTEM_STATE;
	#define IS_MODE_ACTIVE(mode) ((mode == DUTY) || (mode == VELOCITY))

	class MotorSystem{
		lowMotorSystem* low;
		#define CHECK_LOWHANDLER(t) if( t->low == NULL) Error_Handler();

		float duty;

		float current; /** 目標電流 [A] */
		float speed; /** 目標スピード [rad/s] */

		PID::PIDControler speedControler;

		/**
		 * internal parameters
		 */
		MOTORSYSTEM_STATE state;

		/**
		 * internal set duty method.
		 */
		void __setDuty(float){
			CHECK_LOWHANDLER(this);
			this->duty = duty;

			if(this->duty < 0){
				duty = -this->duty;
			}
			this->low->setDuty(duty);
		}

	public:
		MotorSystem();

		/**
		 * initialize
		 */
		returnState init(lowMotorSystem*);

		/**
		 * Change State method.
		 *
		 *
		 */
		void setState(MOTORSYSTEM_STATE state);

		returnState setDuty(float);
		float getDuty(void);

		float getSpeed(void);

		void controlTick(void);

	};
}

#endif /* INC_MOTORSYSTEM_MOTORSYSTEM_HPP_ */
