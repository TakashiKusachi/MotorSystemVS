/** 
 *	@file
 *
 *  @date Created on: 2020/11/07
 *  @Author TakashiKusachi
 */

#ifndef MOTORSYSTEM_ABSTRACT_HPP_
#define MOTORSYSTEM_ABSTRACT_HPP_

#ifdef __cplusplus

#include "./motorsystem_cmd.hpp"
#include "./motorsystem_mode.hpp"
#include "motorsystem_util.hpp"

/**
 *	@namespace
 * 	@brief
 */
namespace nsMotorSystem{

	/**
	 * @class
	 *
	 * モータシステムの基本抽象クラス
	 *
	 * ホストスレーブ関係なく共通して使用し、MotorSystemクラスがなすべき処理をインターフェース示すクラス。
	 * MotorSystemを使う処理（クラス）は、特別な処理をしない限りはこのインターフェースクラスを用いればよいことになる。
	 * インターフェースという概念については、適当に調べてください。
	 */
	class IMotorSystem{
	public:

		/**
		 * begin()
		 *
		 * Begin命令を実行します。Begin命令は、初期化処理が終了するまではブロックします。
		 * ブロックとは、特定の処理が終了するまで関数が戻らないことを示します。
		 */
		virtual void begin(void) = 0;

		/**
		 * set/getMode
		 *
		 * モードの設定・取得関数。
		 *
		 */
		virtual void setMode(MOTORSYSTEM_STATE) = 0;
		virtual MOTORSYSTEM_STATE getMode(void) = 0;

		/**
		 * 各種パラメータ設定
		 */
		virtual returnState setVoltage(float) = 0;
		virtual returnState setPPR(float) = 0;
		virtual returnState setKT(float) = 0;

		/**
		 * 速度制御ゲイン設定関数
		 */
		virtual returnState setVGAIN_K(float) = 0;
		virtual returnState setVGAIN_TI(float) = 0;
		virtual returnState setVGAIN_TD(float) = 0;

		/**
		 * Dutyの設定・取得関数
		 */
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
