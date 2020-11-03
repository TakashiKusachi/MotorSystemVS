# MotorSystem Version STM Code
## 単語の定義

|単語|定義|
|----:|:----:|
|node|MotorSystemそのもの。コントロールされる側|
|host|MotorSystemをコントロールする側|

## STM32CubeIDEプロジェクト 概要
1. [MotorSystem-common](./MotorSystem-common): ホスト・ノード両方で扱われるコード（列挙子やマクロ）をまとめたプロジェクト
1. [MotorSystem-host](./MotorSystem-host): ホスト側のマイコンに依存しない基本コード。
1. [MotorSystem-node](./MotorSystem-node): ノード側のマイコンに依存しない基本コード。
1. [MotorSystem](./MotorSystem): ノード側のマイコンに依存したコードで、実行可能プロジェクト。
1. [HostSample](./HostSample): ノードを制御するホストのサンプルプロジェクト。