#pragma once

#include "type.h"


namespace Cmd
{
/// 移动方向的定义
enum {
	_DIR_UP      = 0,/// 向上
	_DIR_UPRIGHT  = 1,/// 右上
	_DIR_RIGHTUP  = 1,/// 右上
	_DIR_RIGHT    = 2,/// 向右
	_DIR_RIGHTDOWN  = 3,/// 右下
	_DIR_DOWNRIGHT  = 3,/// 右下
	_DIR_DOWN    = 4,/// 向下
	_DIR_DOWNLEFT  = 5,/// 左下
	_DIR_LEFTDOWN  = 5,/// 左下
	_DIR_LEFT    = 6,/// 向左
	_DIR_LEFTUP    = 7,/// 左上
	_DIR_UPLEFT    = 7,/// 左上
	_DIR_WRONG    = 8    /// 错误方向
};

/// 检查某个状态是否设置
inline bool isset_state(const BYTE *state,const int teststate)
{
	return 0 != (state[teststate / 8] & (0xff & (1 << (teststate % 8))));
}

/// 设置某个状态
inline void set_state(BYTE *state,const int teststate)
{
	state[teststate / 8] |= (0xff & (1 << (teststate % 8)));
}

/// 清除某个状态
inline void clear_state(BYTE *state,const int teststate)
{
	state[teststate / 8] &= (0xff & (~(1 << (teststate % 8))));
}

enum
{

	MAX_STATE = 100,
};

}
