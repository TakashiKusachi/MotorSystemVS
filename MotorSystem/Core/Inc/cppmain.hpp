/*
 * cppmain.hpp
 *
 *  Created on: Oct 24, 2020
 *      Author: 嵩
 */

#ifndef INC_CPPMAIN_HPP_
#define INC_CPPMAIN_HPP_

#ifdef __cplusplus
extern "C" {
#endif

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

#endif /* INC_CPPMAIN_HPP_ */
