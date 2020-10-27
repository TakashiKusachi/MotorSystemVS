/*
 * PID.hpp
 *
 *  Created on: Oct 15, 2020
 *      Author: 嵩
 */

#ifndef INC_PID_PID_HPP_
#define INC_PID_PID_HPP_

#include<float.h>

/**
 *
 * header only program
 */

namespace PID{

	class PIDControler{

		/** parameters */
		float K; /** Gain */
		float Ti; /** integral */
		float Td; /** diffe */
		float dt;

		/**  variable */
		float sum_i;
		float befor_e;

	public:
		PIDControler(void){
			this->K = 0.1;
			this->Ti = 0.5;
			this->Td = 0;
			this->dt = 0.1;

			this->sum_i = 0;
			this->befor_e = 0;
		}

		float control(float target,float current){
			float K = this->K;
			float Ti = this->Ti;
			float Td = this->Td;
			float dt = this->dt;
			float befor_e = this->befor_e;

			float e = target - current; /* 偏差 */
			float sum_i = this->sum_i + (e + befor_e) * dt / 2.0; /* 積分計算*/
			float de = (e - befor_e) / dt; /*微分計算*/

			float value = K * (e + sum_i / Ti + de * Td); /*制御量計算*/

			this->sum_i = sum_i;
			this->befor_e = e;

			return value;
		}
	};

}



#endif /* INC_PID_PID_HPP_ */
