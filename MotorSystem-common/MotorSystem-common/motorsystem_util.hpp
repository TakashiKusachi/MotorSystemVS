/*
 * @file motorSystem_util.hpp
 *
 * @author TakashiKusachi
 */

#ifndef INC_MOTORSYSTEM_MOTORSYSTEM_UTIL_HPP_
#define INC_MOTORSYSTEM_MOTORSYSTEM_UTIL_HPP_

/**
 *
 */
namespace nsMotorSystem{

	/**
	 * @brief コマンドの成功・失敗のリターン値
	 * @note 現在つかってない。
	 */
	typedef enum{
		RS_OK,			/**< */
		RS_ERROR,		/**< */
	}returnState;

	/**
	 * @brief エラー原因のenum
	 * @note いまほとんど活用できてない。
	 */
	typedef enum{
		NOT_IMPLEMENTED,		/**< 未実装命令エラー */
		ILLIGAL_MODE_CHANGE,	/**< 不許可状態遷移 */
		NOT_ERROR,				/**< 非エラー */
	}error_state;

}



#endif /* INC_MOTORSYSTEM_MOTORSYSTEM_UTIL_HPP_ */
