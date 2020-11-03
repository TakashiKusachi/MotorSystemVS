# MotorSystem Version STM Code
## 単語の定義

|単語|定義|
|----:|:----:|
|node|MotorSystemそのもの。コントロールされる側|
|host|MotorSystemをコントロールする側|
|プロジェクト|STM32CubeIdeのワークスペース内のプロジェクトのことを指す|

## STM32CubeIDEプロジェクト 概要
1. [MotorSystem-common](./MotorSystem-common): ホスト・ノード両方で扱われるコード（列挙子やマクロ）をまとめたプロジェクト
1. [MotorSystem-host](./MotorSystem-host): ホスト側のマイコンに依存しない基本コード。
1. [MotorSystem-node](./MotorSystem-node): ノード側のマイコンに依存しない基本コード。
1. [MotorSystem](./MotorSystem): ノード側のマイコンに依存したコードで、実行可能プロジェクト。
1. [HostSample](./HostSample): ノードを制御するホストのサンプルプロジェクト。

## ホスト側を開発するために、プロジェクトをインポートする方法

__注__: <details><summary>現時点(2020/11/3)ではSTM32F303にしか対応していません。</summary>将来的にはどうにか対応してほしいです。原因はプロジェクトをマイコンの種類を指定して作成する必要があるからです。使う予定のあるマイコンでそれぞれstatic ライブラリを作成する方法が一つですかね。</details>

