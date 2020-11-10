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

	class IPID{
	public:
		virtual void resetVariable(void) = 0;
		virtual float control(float,float) = 0;
	};

	class PIDControler: public IPID{

		/** parameters */
		float K; /** Gain */
		float Ti; /** integral */
		float Td; /** diffe */
		float dt;

		/**  variable */
		float sum_i;
		float befor_e;

	public:
		PIDControler(float K,float Ti,float Td,float dt){
			this->K = K;
			this->Ti = Ti;
			this->Td = Td;
			this->dt = dt;

			this->resetVariable();
		}

		void setK(float k){
			this->K = k;
		}

		void setTi(float ti){
			this->Ti = ti;
		}

		void setTd(float td){
			this->Td = td;
		}

		float getK(void){
			return this->K;
		}

		float getTi(void){
			return this->Ti;
		}

		float getTd(void){
			return this->Td;
		}

		void resetVariable(void){
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

#define I_CONTROL_DISABLE FLT_MAX


#endif /* INC_PID_PID_HPP_ */
