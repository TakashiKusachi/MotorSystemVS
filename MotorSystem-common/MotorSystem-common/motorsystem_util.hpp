/*
 * motorSystem_util.hpp
 *
 *  Created on: Oct 11, 2020
 *      Author: 嵩
 */

#ifndef INC_MOTORSYSTEM_MOTORSYSTEM_UTIL_HPP_
#define INC_MOTORSYSTEM_MOTORSYSTEM_UTIL_HPP_

namespace nsMotorSystem{

	typedef enum{
		RS_OK,
		RS_ERROR,
	}returnState;

	typedef enum{
		NOT_IMPLEMENTED,
		ILLIGAL_MODE_CHANGE,
		NOT_ERROR,
	}error_state;

}



#endif /* INC_MOTORSYSTEM_MOTORSYSTEM_UTIL_HPP_ */
