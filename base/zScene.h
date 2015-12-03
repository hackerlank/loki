#pragma once
#include <cstddef>
#include "zEntry.h"

/**
 * \brief 场景管理器定义
 */
enum enumSceneRunningState{
	SCENE_RUNNINGSTATE_NORMAL,//正常运行
	SCENE_RUNNINGSTATE_UNLOAD,//正在卸载
	SCENE_RUNNINGSTATE_REMOVE,//正在卸载
};  


struct zScene : public zEntry
{
	private: 
		uint32_t running_state;
	public:
		zScene():running_state(SCENE_RUNNINGSTATE_NORMAL){}
		DWORD getRunningState() const
		{
			return running_state;
		}
		DWORD setRunningState(uint32_t set)
		{
			running_state = set;
			return running_state;
		}

};
