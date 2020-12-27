/**
 * @file PID.hpp
 * @brief PID制御器クラスのヘッダーオンリーコード
 * 
 * @author TakashiKusachi
 */

#ifndef INC_PID_PID_HPP_
#define INC_PID_PID_HPP_

#include<float.h>

/**
 * @brief PID器のnamespace
 */
namespace PID{

	/**
	 * @brief PID制御器のインターフェースクラス
	 */
	class IPID{
	public:
		
		/**
		 * @brief 内部の変数のリセットメソッド
		 */
		virtual void resetVariable(void) = 0;

		/**
		 * @brief 制御量の計算メソッド
		 * @param[in] target 目標値
		 * @param[in] current 現在値
		 * @return 制御量
		 */
		virtual float control(float target, float current) = 0;
	};

	/**
	 * @brief 標準的なPID制御器のクラス
	 * @details
	 * \f$ y=K\left(e+\frac{1}{T_i}\int{edt}+T_d\frac{de}{dt}\right)\f$
	 * 
	 */
	class PIDControler: public IPID{

		/**
		 * @name PID parameters
		 * @{
		 */
		float K; //!< Gain
		float Ti; //!< 積分時間
		float Td; //!< 微分時間
		float dt; //!< 時間幅
		/**
		 * @}
		 */

		/**
		 * @name PID variables
		 * @{
		 */
		float sum_i; //!< 積分値
		float befor_e; //!< 前回の偏差
		/**
		 * @}
		 */

	public:

		/**
		 * @brief コンストラクタ
		 * @param[in] K ゲイン
		 * @param[in] Ti 積分時間
		 * @param[in] Td 微分時間
		 * @param[in] dt 時間幅
		 */
		PIDControler(float K,float Ti,float Td,float dt){
			this->K = K;
			this->Ti = Ti;
			this->Td = Td;
			this->dt = dt;

			this->resetVariable();
		}

		/**
		 * @name Setter
		 * @{
		 */
		/**
		 * @brief Setter Gain
		 * @param[in] K ゲイン
		 */
		void setK(float k){
			this->K = k;
		}

		/**
		 * @brief Setter Ti
		 * @param[in] ti 積分時間
		 */
		void setTi(float ti){
			this->Ti = ti;
		}

		/**
		 * @brief Setter Td
		 * @param[in] td 微分時間
		 */
		void setTd(float td){
			this->Td = td;
		}
		/**
		 * @}
		 */

		/**
		 * @name getter
		 * @{
		 */
		/**
		 * @brief getter Gain
		 * @return Gain
		 */
		float getK(void){
			return this->K;
		}
		/**
		 * @brief getter Ti
		 * @return Ti
		 */
		float getTi(void){
			return this->Ti;
		}
		/**
		 * @brief getter td
		 * @return td
		 */
		float getTd(void){
			return this->Td;
		}
		/**
		 * @}
		 */

		/**
		 *
		 */
		void resetVariable(void){
			this->sum_i = 0;
			this->befor_e = 0;
		}

		/**
		 * @brief 制御量の計算メソッド
		 * @param[in] target 目標値
		 * @param[in] current 現在値
		 * @return 制御量
		 *
		 * @details 標準的なPID制御を行います。
		 */
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

/** 
 * @brief 積分制御を無効にするための定数
 * @details 積分時間はゲインの逆襲になるので、0を入力するとゲインとしては無限大になってしまいます（0除算）
 * なので、無効にするためには、0ではなく最大値を入れてあげることで実質無効にできるという原理。
 */
#define I_CONTROL_DISABLE FLT_MAX


#endif /* INC_PID_PID_HPP_ */
