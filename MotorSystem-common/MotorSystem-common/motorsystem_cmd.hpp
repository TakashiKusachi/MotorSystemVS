/*
 * motorsystem-cmd.hpp
 *
 *  Created on: Nov 3, 2020
 *      Author: 嵩
 */

#ifndef MOTORSYSTEM_CMD_HPP_
#define MOTORSYSTEM_CMD_HPP_

#ifdef __cplusplus
extern "C"{
#endif

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
}
#endif

#endif /* MOTORSYSTEM_CMD_HPP_ */
