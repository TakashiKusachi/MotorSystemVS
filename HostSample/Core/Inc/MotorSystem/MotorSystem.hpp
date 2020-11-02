/*
 * MotorSystem.hpp
 *
 *  Created on: Nov 2, 2020
 *      Author: 嵩
 */

#ifndef INC_MOTORSYSTEM_HPP_
#define INC_MOTORSYSTEM_HPP_

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

#define CAN_MESSAGE_GET_CMD(stdid) (stdid >> 4)
#define CAN_MESSAGE_GET_ID(stdid) (stdid & 0x0F)
#define CAN_MESSAGE_MAKE_CMD(cmd,id) ((cmd << 4) | (id & 0x000f))

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

#endif /* INC_MOTORSYSTEM_HPP_ */
