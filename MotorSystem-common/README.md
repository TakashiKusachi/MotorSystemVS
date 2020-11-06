# MotorSystem-Common ライブラリ
MotorSystemのホストとノードで共通のコードをまとめたSTM32CubeIDEのプロジェクト

主に、ホスト―ノード間の通信に使われる定数（CMDやMODEなど）が定義されています。

## 各種ヘッダー
### [motorsystem_cmd.hpp](./MotorSystem-common/motorsystem_cmd.hpp)
* enum<br>
    * typedef enum MOTORSYSTEM_CMD;
* macros<br>
    * <details><summary>CAN_MESSAGE_GET_CMD(stdid)</summary>取得したCANメッセージのSIDからコマンド部分を抽出するマクロ</details>
    * <details><summary>CAN_MESSAGE_GET_ID(stdid)</summary>取得したCANメッセージのSIDからデバイスのIDを抽出するマクロ</details>
    * <details><summary>CAN_MESSAGE_MAKE_CMD(cmd,id)</summary>コマンドとIDからCANメッセージのSIDを生成するマクロ</details>

### [motorsystem_mode.hpp](./MotorSystem-common/motorsystem_mode.hpp)
* enum<br>
    * typedef enum MOTORSYSTEM_STATE;
* macros<br>
    * <details><summary>IS_MODE_ACTIVE</summary>動作中(DUTY or VELOCITY)であるかをboolで返すマクロ</details>
    * <details><summary>IS_NOT_BEGIN(mode)</summary>beginされていないかどうかをboolで返すマクロ</details>

### [motorsystem_abstract.hpp](./MotorSystem-common/motorsystem_abstract.hpp)
* namespace
    * MotorSystem

* class IMotorSystem;