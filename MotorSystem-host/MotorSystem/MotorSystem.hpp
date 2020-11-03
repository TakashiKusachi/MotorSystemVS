/*
 * MotorSystem.hpp
 *
 *  Created on: Nov 3, 2020
 *      Author: 嵩
 */

#ifndef MOTORSYSTEM_HPP_
#define MOTORSYSTEM_HPP_

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

	class MotorSystem{
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

		volatile bool recive_current;
		float current;

		void sendMessage(MOTORSYSTEM_CMD cmd,int rtr,int dlc,unsigned char* data);

	public:
		MotorSystem();

		/**
		 * initialize
		 */
		void init(lowMotorSystem*,int);

		void begin(void);

		/**
		 * Change State method.
		 *
		 *
		 */
		void setMode(MOTORSYSTEM_STATE state);
		MOTORSYSTEM_STATE getMode(void);

		void setDuty(float);
		float getDuty(void);

		void setVelocity(float);
		float getVelocity(void);

		float getCurrent(void);

		void setVoltage(float);

		void parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data);

		void reset(void);
		int getID(void);
	};
}


#endif


#endif /* MOTORSYSTEM_HPP_ */
