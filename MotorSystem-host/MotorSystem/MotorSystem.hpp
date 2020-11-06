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

#ifdef __cplusplus
/** not export c source code */
namespace MotorSystem{

	class MotorSystem;

	class lowMotorSystem{
	public:
		virtual void sendMessage(unsigned long sid,unsigned long rtr,unsigned long dlc, unsigned char* data) = 0;
	};

	class MotorSystem:public IMotorSystem{
		lowMotorSystem* low;
		#define CHECK_LOWHANDLER(t) if( t->low == NULL) this->low->ErrorHandler();

		int id;

		volatile bool recive_begin;

		volatile bool recive_mode;
		MOTORSYSTEM_STATE mode;

		volatile bool recive_duty;
		float duty;

		volatile bool recive_velocity;
		float velocity;

		volatile bool recive_torque;
		float torque;

		volatile bool recive_current;
		float current;

		void sendMessage(MOTORSYSTEM_CMD cmd,int rtr,int dlc,unsigned char* data);

	public:
		MotorSystem();

		/**
		 * initialize
		 */
		void init(lowMotorSystem*,int);

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
		returnState setKT(float)  override;

		returnState setVGAIN_K(float) override;
		returnState setVGAIN_TI(float) override;
		returnState setVGAIN_TD(float) override;

		returnState setDuty(float) override;
		float getDuty(void) override;

		returnState setVelocity(float) override;
		float getVelocity(void) override;

		returnState setTorque(float) override;
		float getTorque(void) override;

		float getCurrent(void) override;
;

		void parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data);

		void reset(void);
		int getID(void);
	};
}


#endif


#endif /* MOTORSYSTEM_HPP_ */
