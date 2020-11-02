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
	INITIALIZED,
	READY,
	DUTY,
	VELOCITY,

	SYSTEM_RESET,
}MOTORSYSTEM_STATE;
#define IS_MODE_ACTIVE(mode) ((mode == DUTY) || (mode == VELOCITY))
#define IS_NOT_BEGIN(mode) ((mode == NOT_INITIALIZED) || (mode==INITIALIZED))

typedef enum{
									/** (not use[1] proprity[2] W/^R[1] cmd[4]) */
	SET_VELOCITY = 0x00,			/** 0b0 00 0 0000 */
	SET_TORQUE = 0x01,				/** 0b0 00 0 0001 */
	SET_DUTY = 0x02,				/** 0b0 00 0 0010 */
	// = 0x03,
	SET_MODE = 0x04,				/** 0b0 00 0 0100 */
	// 0x05 ~ 0x13
	GET_MODE = 0x14,				/** 0b0 00 1 0100 */
	SET_VCC = 0x45,					/** 0b0 10 0 0101 */
	SET_PPR = 0x46,					/** 0b0 10 0 0110 */
	SET_KT = 0x47,					/** 0b0 10 0 0111 */
	SET_VGAIN_K = 0x48,				/** 0b0 10 0 1000 */
	SET_VGAIN_TI = 0x49,			/** 0b0 10 0 1001 */
	SET_VGAIN_TD = 0x4a,			/** 0b0 10 0 1010 */
	SET_CGAIN_K = 0x4c,				/** 0b0 10 0 1100 */
	SET_CGAIN_TI = 0x4d,			/** 0b0 10 0 1101 */
	SET_CGAIN_TD = 0x4e,			/** 0b0 10 0 1110 */
	BEGIN = 0x54,					/** 0b0 10 1 0100 */
	GET_VELOCITY=0x70,				/** 0b0 11 1 0000 */
	GET_TORQUE = 0x71,				/** 0b0 11 1 0001 */
	GET_DUTY = 0x72,				/** 0b0 11 1 0010 */
	GET_CURRENT = 0x73,				/** 0b0 11 1 0011 */
	GET_STATE = 0x74,				/** 0b0 11 1 0100 */
	GET_V_K = 0x78,					/** 0b0 11 1 1000 */
	GET_V_TI = 0x79,				/** 0b0 11 1 1001 */
	GET_V_TD = 0x7A,				/** 0b0 11 1 1010 */
	GET_C_K = 0x7C,					/** 0b0 11 1 1100 */
	GET_C_TI = 0x7D,				/** 0b0 11 1 1101 */
	GET_C_TD = 0x7E,				/** 0b0 11 1 1110 */

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

	class MotorSystem{
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

		float getCurrent(void);

		void setVoltage(float);

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
