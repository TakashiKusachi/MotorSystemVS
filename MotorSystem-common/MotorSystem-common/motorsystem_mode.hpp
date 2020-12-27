/*
 * @file motorsystem-cmd.hpp
 *
 * @author TakashiKusachi
 */

#ifndef MOTORSYSTEM_MODE_HPP_
#define MOTORSYSTEM_MODE_HPP_

#ifdef __cplusplus
extern "C"{
#endif

/**
 *
 */
namespace nsMotorSystem{

/**
 * @brief モータの状態ならびにモードのenum
 * @details 詳細はwiki見た方がいい。
 */
typedef enum{
	NOT_INITIALIZED,	/**< 未初期化状態 */
	INITIALIZED,		/**< 初期化済み状態(Not begin) */
	READY,				/**< Begin後 */
	DUTY,				/**< Duty制御モード */
	VELOCITY,			/**< 速度制御モード */

	ERROR_MODE,			/**< エラー発生 */
	SYSTEM_RESET,		/**< システムリセット */
}MOTORSYSTEM_STATE;

}

#define IS_MODE_ACTIVE(mode) ((mode == DUTY) || (mode == VELOCITY))
#define IS_NOT_BEGIN(mode) ((mode == NOT_INITIALIZED) || (mode==INITIALIZED))

#ifdef __cplusplus
}
#endif

#endif /* MOTORSYSTEM_MODE_HPP_ */
