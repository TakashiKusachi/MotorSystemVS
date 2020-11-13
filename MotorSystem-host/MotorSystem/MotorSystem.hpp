/**
 * @file MotorSystem.hpp
 * @author Takashi Kusachi
 */

#ifndef MOTORSYSTEM_HPP_
#define MOTORSYSTEM_HPP_

#include "MotorSystem-common/motorsystem_abstract.hpp"
#include "MotorSystem-common/motorsystem_cmd.hpp"
#include "MotorSystem-common/motorsystem_mode.hpp"

#ifdef __cplusplus

/**
 */
namespace nsMotorSystem{
/**
 * @brief モータシステムのホスト側のネームスペース
 */
namespace nsHost{

	class MotorSystem;

	/**
	 * @brief nsHost::MotorSystemクラスが使用するハードウェアリソースのインターフェース
	 * @details
	 */
	class lowMotorSystem{
	public:
		/**
		 * @brief CANメッセージ送信関数
		 * @param [in] sid メッセージのスタンダードID
		 * @param [in] rtr Remote Transmission Request 0(Data frame)/1(Remote frame)
		 * @param [in] dlc データ長
		 * @param [in] data データ
		 * @details MotorSystemは現在の仕様は標準IDで構成します。
		 */
		virtual void sendMessage(unsigned long sid,unsigned long rtr,unsigned long dlc, unsigned char* data) = 0;

		/**
		 * @bried ハードウェア側でのエラー処理
		 * @details　ハードウェア側でエラーが発生したときの処理。例えば、LEDを点滅させたり、音を鳴らしたり。モータを止めるのはMotorSystemがするのでそれ以外の処理。
		 * @note この処理は、CANの受信割り込み中でも発生しうるため、ブロッキングを行うと、以後のCANメッセージが受け取れなくなります。このため、ブロッキングはしないでください。
		 */
		virtual void ErrorHandler(void) = 0;
	};

	/**
	 * @brief Node側のMotorSystemの実装
	 * @details
	 * @note get命令にタイムアウト処理を実装したほうがいいかもしれない。
	 */
	class MotorSystem:public IMotorSystem{
		lowMotorSystem* low; /**< @brief 必要なハードウェア処理を担うインターフェースのインスタンス */
		#define CHECK_LOWHANDLER(t) if( t->low == NULL) this->low->ErrorHandler();

		int id; /**< @brief ハードウェアID */

		volatile bool recive_begin; 	/**< @brief begin命令のリターン検出フラグ　*/

		volatile bool recive_mode;		/**< @brief getMode命令のリターン検出フラグ */
		MOTORSYSTEM_STATE mode;			/**< @brief 取得したモード */

		volatile bool recive_duty;		/**< @brief getDuty命令のリターン検出フラグ */
		float duty;

		volatile bool recive_velocity;
		float velocity;

		volatile bool recive_torque;
		float torque;

		volatile bool recive_current;
		float current;

		void sendMessage(MOTORSYSTEM_CMD cmd,int rtr,int dlc,unsigned char* data);

	public:
		MotorSystem();

		/**
		 * initialize
		 */
		void init(lowMotorSystem*,int);

		void begin(void) override;

		/**
		 * Change State method.
		 *
		 *
		 */
		void setMode(MOTORSYSTEM_STATE state) override;
		MOTORSYSTEM_STATE getMode(void) override;

		returnState setVoltage(float) override;
		returnState setPPR(float) override;
		returnState setKT(float)  override;

		returnState setVGAIN_K(float) override;
		returnState setVGAIN_TI(float) override;
		returnState setVGAIN_TD(float) override;

		returnState setDuty(float) override;
		float getDuty(void) override;

		returnState setVelocity(float) override;
		float getVelocity(void) override;

		returnState setTorque(float) override;
		float getTorque(void) override;

		float getCurrent(void) override;

		void parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data);

		void reset(void);
		int getID(void);
	};
}
}


#endif


#endif /* MOTORSYSTEM_HPP_ */
