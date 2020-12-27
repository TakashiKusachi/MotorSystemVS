/*
 * @file MotorSystem.hpp
 * @author Takashi Kusachi
 */

#ifndef MOTORSYSTEM_HPP_
#define MOTORSYSTEM_HPP_

#include "MotorSystem-common/motorsystem_abstract.hpp"
#include "MotorSystem-common/motorsystem_cmd.hpp"
#include "MotorSystem-common/motorsystem_mode.hpp"
#include "MotorSystem-common/motorsystem_util.hpp"
#include "pid/PID.hpp"

#ifdef __cplusplus

/**
 *
 */
namespace nsMotorSystem{

/**
 * @brief モータシステムのノード側のネームスペース
 */
namespace nsNode{

	/**
	 * @brief nsNode::MotorSystemクラスが使用するハードウェアリソースのインターフェース
	 * @details
	 */
	class lowMotorSystem{
	public:

		/**
		 * @brief 初期化処理
		 * @details begin命令にともなう必要なハードウェア初期化処理を実行します。ADCの初期化など。
		 */
		virtual void begin(void) = 0;

		/**
		 * @brief ID取得関数
		 * @details ハードウェアに接続されるスイッチからIDを取得するメソッド。
		 *
		 */
		virtual int getID(void) = 0;

		/**
		 * @brief Duty設定関数
		 * @param [in] duty[%] 0~100%。IMotorSystemクラスのsetDutyと違って0からになっています。
		 * @details ペリフェラルを用いてPWMのdutyを設定します。
		 * @note 入力の幅に注意してください。
		 */
		virtual void setDuty(float) = 0;

		/**
		 * @brief 回転方向設定関数
		 * @param [in] direction 0 or 1.どっちがどっちに回転するかは配線によるのでわからん。
		 * @details ペリフェラル(GPIO etc...)を用いて回転方向を設定します。
		 */
		virtual void setDirection(int) = 0;

		/**
		 * @brief エンコーダのpulse数の設定関数
		 * @param [in] ppr pulse per revolution
		 * @details 回転速度算出に必要なpulse per revolutionを設定します。
		 */
		virtual void setPPR(int) = 0;

		/**
		 * @brief 電流取得関数
		 * @return 電流[A]
		 * @details
		 */
		virtual float getCurrent(void) = 0;

		/**
		 * @brief 速度取得関数
		 * @return velocity [rad/s]
		 */
		virtual float getSpeed(void) = 0;

		/**
		 * @brief CANメッセージ送信関数
		 * @param [in] sid メッセージのスタンダードID
		 * @param [in] rtr Remote Transmission Request 0(Data frame)/1(Remote frame)
		 * @param [in] dlc データ長
		 * @param [in] data データ
		 * @details MotorSystemは現在の仕様は標準IDで構成します。
		 */
		virtual void sendMessage(unsigned long sid,unsigned long rtr,unsigned long dlc,unsigned char* data) = 0;

		/**
		 * @bried ハードウェア側でのエラー処理
		 * @details　ハードウェア側でエラーが発生したときの処理。例えば、LEDを点滅させたり、音を鳴らしたり。モータを止めるのはMotorSystemがするのでそれ以外の処理。
		 * @note この処理は、CANの受信割り込み中でも発生しうるため、ブロッキングを行うと、以後のCANメッセージが受け取れなくなります。このため、ブロッキングはしないでください。
		 */
		virtual void ErrorHandler(void) = 0;

		/**
		 * @brief マイコンのリセット
		 *
		 * @details Hostからの指示でマイコンのリセットを行います。ソフトウェアリセットを掛けてください。
		 */
		virtual void reset(void)=0;
	};

	/**
	 * @brief Node側のMotorSystemの実装
	 * @details
	 */
	class MotorSystem:public IMotorSystem{
		lowMotorSystem* low; /**< @brief 必要なハードウェア処理を担うインターフェースのインスタンス */
		#define CHECK_LOWHANDLER(t) if( t->low == NULL) this->low->ErrorHandler();

		float duty; /**< @brief 指定Duty */

		float voltage; /**< @brief 目標電圧[V] */
		float current; /**< @brief目標電流 [A] */
		float speed; /**< @brief 目標スピード [rad/s] */

		float supplyVoltage; /**< @brief供給電圧[V] */

		PID::PIDControler speedControler; /**< @brief 速度制御PIDのインスタンス */

		/**
		 * @name internal parameters
		 * @{
		 */
		MOTORSYSTEM_STATE state; /**< @brief 状態（モード） */
		error_state estate; /**< @brief エラーステート */
		/**
		 * @}
		 */

		/**
		 * @name internal methods
		 * @brief クラス内部でのみ使用するメソッド
		 * @{
		 */
		/**
		 * @brief （内部用）duty設定関数
		 * @details この関数はハードウェアにdutyを設定するためのリミット関数の役割を持っています。
		 * @note this.setDutyはDutyモードで外部からdutyを設定するためメソッドです。velocityモード他モードでは、こちらの関数を使用します。
		 *
		 */
		void __setDuty(float duty){
			CHECK_LOWHANDLER(this);
			this->duty = duty;

			if(this->duty < 0){
				this->low->setDirection(1);
				duty = -this->duty;
			}else{
				this->low->setDirection(0);
			}
			this->low->setDuty(duty);
		}

		/**
		 * @brief （内部用）voltage設定関数
		 * @details 出力電圧を受け取り、設定された電源電圧からdutyに変換して出力します。
		 * @note this.setVoltageは電源電圧の設定で、こちらは出力関数です。
		 */
		void __setVoltage(float vol){
			CHECK_LOWHANDLER(this);
			this->voltage = vol;
			float duty = vol / this->supplyVoltage;
			this->__setDuty(duty);
		}
		/**
		 * @}
		 */

	public:

		/**
		 * @brief コンストラクタ
		 */
		MotorSystem();

		/**
		 * @brief 初期化関数
		 */
		returnState init(lowMotorSystem*);

		/**
		 * @name IMotorSystemの実装
		 * @{
		 */
		void begin(void) override;

		void setMode(MOTORSYSTEM_STATE state) override;
		MOTORSYSTEM_STATE getMode(void) override;

		returnState setVoltage(float) override;
		returnState setPPR(float) override;
		returnState setKT(float) override;

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
		/**
		 * @}
		 */

		/**
		 * @brief 制御を行う周期処理
		 * @details
		 */
		void controlTick(void);

		/**
		 * @brief CANメッセージの受け取りメソッド
		 */
		void parseCANMessage(unsigned long id, bool rtr,unsigned char dlc,unsigned char* data);

		/**
		 * @brief エラーが発生したときにhostにエラーメッセージを送るための関数
		 * @note private の方がいいかも
		 */
		void sendErrorMessage(void);
		/**
		 * @brief 実装外の処理を行ったときに行うメソッド
		 * @note マクロにNOT_IMPLEMENTEDがあるので基本はそちらを用いること
		 */
		void NotImplemented(const char*,long);
		/**
		 * @brief 許可されていない状態遷移を行ったときに行うメソッド
		 * @note マクロにILLIGAL_MODE_CHANGEがあるので基本はそちらを用いること
		 */
		void IlligalModeChange(const char*,long);

	};
}
}

#define MOTORSYSTEM_DEFAULT_K 0.1
#define MOTORSYSTEM_DEFAULT_Ti 0.5
#define MOTORSYSTEM_DEFAULT_Td 0.0
#define MOTORSYSTEM_DEFAULT_dt 0.5

#endif



#endif /* MOTORSYSTEM_HPP_ */
