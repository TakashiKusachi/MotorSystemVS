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

#ifdef __cplusplus
extern "C"{
#endif
/** export c source code */

typedef enum{
	NOT_INITIALIZED,
	READY,
	DUTY,
	VELOCITY,
}MOTORSYSTEM_STATE;
#define IS_MODE_ACTIVE(mode) ((mode == DUTY) || (mode == VELOCITY))

typedef enum{
	SET_VELOCITY = 0x00,
	SET_TORQUE = 0x01,
	SET_DUTY = 0x02,
	// = 0x03,
	SET_MODE = 0x04,
	// 0x05 ~ 0x13
	GET_MODE = 0x14,
	BEGIN = 0x44,
	SET_VCC = 0x45,
	SET_PPR = 0x46,
	SET_KT = 0x47,
	SET_VGAIN_K = 0x48,
	SET_VGAIN_TI = 0x49,
	SET_VGAIN_TD = 0x4a,
	SET_CGAIN_K = 0x4c,
	SET_CGAIN_TI = 0x4d,
	SET_CGAIN_TD = 0x4e,
	GET_VELOCITY=0x70,
	GET_TORQUE = 0x71,
	GET_DUTY = 0x72,
	GET_CURRENT = 0x73,
	GET_STATE = 0x74,
	GET_V_K = 0x78,
	GET_V_TI = 0x79,
	GET_V_TD = 0x7A,
	GET_C_K = 0x7C,
	GET_C_TI = 0x7D,
	GET_C_TD = 0x7E,

}MOTORSYSTEM_CMD;

#define GET_CMD(stdid) (stdid >> 4)
#define MAKE_CMD(cmd,id) ((cmd << 4) | (id & 0x000f))

#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
/** not export c source code */
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
	};

	class MotorSystem{
		lowMotorSystem* low;
		#define CHECK_LOWHANDLER(t) if( t->low == NULL) this->low->ErrorHandler();

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

	public:
		MotorSystem();

		/**
		 * initialize
		 */
		returnState init(lowMotorSystem*);

		void begin(void);

		/**
		 * Change State method.
		 *
		 *
		 */
		void setMode(MOTORSYSTEM_STATE state);
		MOTORSYSTEM_STATE getMode(void);

		returnState setDuty(float);
		float getDuty(void);

		returnState setVelocity(float);
		float getVelocity(void);

		void controlTick(void);
		void parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data);

	};
}

#define MOTORSYSTEM_DEFAULT_K 0.1
#define MOTORSYSTEM_DEFAULT_Ti 0.5
#define MOTORSYSTEM_DEFAULT_Td 0.0
#define MOTORSYSTEM_DEFAULT_dt 0.5

#endif

#endif /* INC_MOTORSYSTEM_MOTORSYSTEM_HPP_ */
