#pragma once

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
};
