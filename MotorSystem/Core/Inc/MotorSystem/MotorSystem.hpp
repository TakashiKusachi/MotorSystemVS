/*
 * MotorSystem.hpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#ifndef INC_MOTORSYSTEM_MOTORSYSTEM_HPP_
#define INC_MOTORSYSTEM_MOTORSYSTEM_HPP_

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
		 */
		virtual void setDuty(float) = 0;

		/**
		 * get current [A].
		 */
		virtual float getCurrent(void) = 0;

		/**
		 * get speed [rad. / s].
		 */
		virtual float getSpeed(void) = 0;
	};

	class MotorSystem{
		lowMotorSystem* low;

		float duty;

		float current; /** 目標速度 */
		float speed; /** 目標スピード */

		PID::PIDControler speedControler;

		/**
		 * internal parameters
		 */
		enum{
			NOT_INITIALIZE,
			READY,
		}state;

	public:
		MotorSystem();
		returnState init(lowMotorSystem*);

		returnState setDuty(float);
		float getDuty(void);

		void controlTick(void);

	};


	#define CHECK_LOWHANDLER(t) if( t->low == NULL) Error_Handler();
}

#endif /* INC_MOTORSYSTEM_MOTORSYSTEM_HPP_ */
