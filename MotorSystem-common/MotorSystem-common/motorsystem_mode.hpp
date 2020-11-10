/*
 * motorsystem-cmd.hpp
 *
 *  Created on: Nov 3, 2020
 *      Author: 嵩
 */

#ifndef MOTORSYSTEM_MODE_HPP_
#define MOTORSYSTEM_MODE_HPP_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum{
	NOT_INITIALIZED,
	INITIALIZED,
	READY,
	DUTY,
	VELOCITY,

	ERROR_MODE,
	SYSTEM_RESET,
}MOTORSYSTEM_STATE;

#define IS_MODE_ACTIVE(mode) ((mode == DUTY) || (mode == VELOCITY))
#define IS_NOT_BEGIN(mode) ((mode == NOT_INITIALIZED) || (mode==INITIALIZED))

#ifdef __cplusplus
}
#endif

#endif /* MOTORSYSTEM_MODE_HPP_ */
