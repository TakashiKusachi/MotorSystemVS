# MotorSystem-Common ライブラリ
MotorSystemのホストとノードで共通のコードをまとめたSTM32CubeIDEのプロジェクト

主に、ホスト―ノード間の通信に使われる定数（CMDやMODEなど）が定義されています。

## 各種ヘッダー
### [motorsystem_cmd.hpp](./MotorSystem-common/motorsystem_cmd.hpp)
* enum<br>
    * typedef enum MOTORSYSTEM_CMD;
* macros<br>
    * CAN_MESSAGE_GET_CMD(stdid)
    * CAN_MESSAGE_GET_ID(stdid)
    * CAN_MESSAGE_MAKE_CMD(cmd,id)