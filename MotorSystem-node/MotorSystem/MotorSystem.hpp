/*
 * MotorSystem.hpp
 *
 *  Created on: Nov 3, 2020
 *      Author: 嵩
 */

#ifndef MOTORSYSTEM_HPP_
#define MOTORSYSTEM_HPP_

#include "MotorSystem-common/motorsystem_abstract.hpp"
#include "MotorSystem-common/motorsystem_cmd.hpp"
#include "MotorSystem-common/motorsystem_mode.hpp"
#include "MotorSystem-common/motorsystem_util.hpp"
#include "pid/PID.hpp"

#ifdef __cplusplus
/** not export c source code */
namespace MotorSystem{

	/**
	 * Interface of peripheral for MotorSystem.
	 */
	class lowMotorSystem{
	public:

		/**
		 *
		 */
		virtual void begin(void) = 0;

		/**
		 *
		 */
		virtual int getID(void) = 0;

		/**
		 * set Duty [%].
		 *
		 * Args:
		 * 		duty (float): duty
		 */
		virtual void setDuty(float) = 0;

		/**
		 *
		 */
		virtual void setDirection(int) = 0;

		/**
		 *
		 */
		virtual void setPPR(int) = 0;

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
		virtual void sendMessage(unsigned long sid,unsigned long rtr,unsigned long dlc,unsigned char* data) = 0;

		/**
		 *
		 */
		virtual void NotImplemented(const char*,int) = 0;

		/**
		 *
		 */
		virtual void ErrorHandler(void) = 0;

		virtual void reset(void)=0;
	};

	class MotorSystem:public IMotorSystem{
		lowMotorSystem* low;
		#define CHECK_LOWHANDLER(t) if( t->low == NULL) this->low->ErrorHandler();

		float duty; /** 指定電圧 */

		float voltage; /** 目標電圧[V] */
		float current; /** 目標電流 [A] */
		float speed; /** 目標スピード [rad/s] */

		float supplyVoltage; /** 供給電圧[V] */

		PID::PIDControler speedControler;

		/**
		 * internal parameters
		 */
		MOTORSYSTEM_STATE state;

		/**
		 * internal set duty method.
		 */
		void __setDuty(float duty){
			CHECK_LOWHANDLER(this);
			this->duty = duty;

			if(this->duty < 0){
				this->low->setDirection(1);
				duty = -this->duty;
			}else{
				this->low->setDirection(0);
			}
			this->low->setDuty(duty);
		}

		/**
		 *
		 */
		void __setVoltage(float vol){
			CHECK_LOWHANDLER(this);
			float duty = vol / this->supplyVoltage;
			this->__setDuty(duty);
		}

	public:
		MotorSystem();

		/**
		 * initialize
		 */
		returnState init(lowMotorSystem*);

		void begin(void) override;

		/**
		 * Change State method.
		 *
		 *
		 */
		void setMode(MOTORSYSTEM_STATE state) override;
		MOTORSYSTEM_STATE getMode(void) override;

		returnState setVoltage(float) override;
		returnState setPPR(float) override;
		returnState setKT(float) override;

		returnState setVGAIN_K(float) override;
		returnState setVGAIN_TI(float) override;
		returnState setVGAIN_TD(float) override;

		returnState setDuty(float);
		float getDuty(void);

		returnState setVelocity(float);
		float getVelocity(void);

		returnState setTorque(float) override;
		float getTorque(void) override;

		float getCurrent(void) override;

		void controlTick(void);
		void parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data);

	};
}

#define MOTORSYSTEM_DEFAULT_K 0.1
#define MOTORSYSTEM_DEFAULT_Ti 0.5
#define MOTORSYSTEM_DEFAULT_Td 0.0
#define MOTORSYSTEM_DEFAULT_dt 0.5

#endif



#endif /* MOTORSYSTEM_HPP_ */
