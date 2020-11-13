/** 
 * @file
 * @brief MotorSystemのホストノード同一の基本クラス
 * @author TakashiKusachi
 */

#ifndef MOTORSYSTEM_ABSTRACT_HPP_
#define MOTORSYSTEM_ABSTRACT_HPP_

#ifdef __cplusplus

#include "./motorsystem_cmd.hpp"
#include "./motorsystem_mode.hpp"
#include "motorsystem_util.hpp"

/**
 * 	@brief MotorSystemの名前空間
 */
namespace nsMotorSystem{

	/**
	 * @brief モータシステムの基本抽象クラス
	 * @details
	 * ホストスレーブ関係なく共通して使用し、MotorSystemクラスがなすべき処理をインターフェース示すクラス。
	 * MotorSystemを使う処理（クラス）は、特別な処理をしない限りはこのインターフェースクラスを用いればよいことになる。
	 * インターフェースという概念については、適当に調べてください。
	 * @note 実際にその関数が使えるかどうかは、実装クラスによります。なので、実装クラスでそのメソッドがサポートされているか確認してください。
	 */
	class IMotorSystem{
	public:

		/**
		 * @brief 初期化関数
		 *
		 * @details
		 * Begin命令を実行します。Begin命令は、初期化処理が終了するまではブロックします。
		 * ブロックとは、特定の処理が終了するまで関数が戻らないことを示します。
		 */
		virtual void begin(void) = 0;

		/**
		 * @brief モードの設定関数。
		 * @param[in] mode モードを設定します。
		 * @details MotorSystemのモード遷移させます。モードの遷移には規則があり、規則外の移動を行うとエラーが発生して
		 * モータを停止させます。
		 * @note エラー時にexceptionを生成させた方がいいかもね。
		 */
		virtual void setMode(MOTORSYSTEM_STATE) = 0;

		/**
		 * @brief モードの取得関数
		 * @return mode
		 */
		virtual MOTORSYSTEM_STATE getMode(void) = 0;

		/**
		 * @name Parameter setter
		 * @{
		 */
		/**
		 * @brief 電源電圧設定関数
		 * @param[in] vol voltage[V]
		 */
		virtual returnState setVoltage(float) = 0;

		/**
		 * @brief ロータリーエンコーダのパルス数設定関数
		 * @param[in] ppr pulse per revolution
		 */
		virtual returnState setPPR(float) = 0;

		/**
		 * @brief トルク定数設定関数
		 * @param[in] Kt torque constans[mNm/A]
		 */
		virtual returnState setKT(float) = 0;
		/**
		 * @}
		 */

		/**
		 * @name
		 * @{
		 */
		/**
		 * @brief 速度制御ゲインの設定関数
		 * @param[in] K Gain
		 */
		virtual returnState setVGAIN_K(float) = 0;
		/**
		 * @brief 速度制御の積分時間設定関数
		 * @param[in] Ti 積分時間
		 */
		virtual returnState setVGAIN_TI(float) = 0;
		/**
		 * @bried 速度制御の微分時間の設定関数
		 * @param[in] Td 微分時間
		 */
		virtual returnState setVGAIN_TD(float) = 0;
		/**
		 * @}
		 */

		/**
		 * @name コントロールメソッド
		 * @{
		 */
		/**
		 * @brief Duty設定関数
		 * @param[in] duty[%] dutyはfloat型で-100~100までの値で設定してください。
		 * @note この関数はMode=Dutyでなければ、エラーが発生します（現状）。
		 */
		virtual returnState setDuty(float) = 0;
		/**
		 * @brief Duty取得関数
		 * @return duty[%] dutyはfloat型ですが、現状の仕様では、-100~100に収めるかは実装しだいとします。
		 */
		virtual float getDuty(void) = 0;

		/**
		 * @brief 速度設定関数
		 * @return velocity[rad/s]
		 * @note この関数はMode=Velocityで無ければエラーが発生します。
		 */
		virtual returnState setVelocity(float) = 0;
		/**
		 * @brief 速度取得関数
		 * @return velocity[rad/s]
		 */
		virtual float getVelocity(void) = 0;

		/**
		 * @brief トルク設定関数
		 * @param torque[mNm] トルク。
		 * @note この関数はMode=Torqueでなければエラーが発生します。
		 */
		virtual returnState setTorque(float) = 0;
		/**
		 * @brief トルク取得関数
		 * @return torque[mNm]
		 */
		virtual float getTorque(void) = 0;

		/**
		 * @brief 電流取得関数
		 * @return current[A]
		 */
		virtual float getCurrent(void) = 0;
		/**
		 * @}
		 */
	};
}

#endif

#endif /* MOTORSYSTEM_ABSTRACT_HPP_ */
