/*
 * motorsytem_abstract.hpp
 *
 *  Created on: 2020/11/07
 *      Author: 嵩
 */

#ifndef MOTORSYSTEM_ABSTRACT_HPP_
#define MOTORSYSTEM_ABSTRACT_HPP_

#ifdef __cplusplus

#include "./motorsystem_cmd.hpp"
#include "./motorsystem_mode.hpp"
#include "motorsystem_util.hpp"

#define interface class
namespace MotorSystem{

	interface IMotorSystem{
	public:
		virtual void begin(void) = 0;

		virtual void setMode(MOTORSYSTEM_STATE) = 0;
		virtual MOTORSYSTEM_STATE getMode(void) = 0;

		virtual returnState setVoltage(float) = 0;
		virtual returnState setPPR(float) = 0;
		virtual returnState setKT(float) = 0;

		virtual returnState setVGAIN_K(float) = 0;
		virtual returnState setVGAIN_TI(float) = 0;
		virtual returnState setVGAIN_TD(float) = 0;

		virtual returnState setDuty(float) = 0;
		virtual float getDuty(void) = 0;

		virtual returnState setVelocity(float) = 0;
		virtual float getVelocity(void) = 0;

		virtual returnState setTorque(float) = 0;
		virtual float getTorque(void) = 0;

		virtual float getCurrent(void) = 0;
	};
}

#endif

#endif /* MOTORSYSTEM_ABSTRACT_HPP_ */
