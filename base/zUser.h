#pragma once
#include "zSceneEntry.h"
#include "zMutex.h"

/**
* \brief 角色管理器定义
*/
/**
* \brief 角色定义类,有待扩充
*/
struct zUser:public zSceneEntry
{
	zUser():zSceneEntry(SceneEntry_Player)
	{
	}
	void lock()
	{
		mlock.lock();
	}

	void unlock()
	{
		mlock.unlock();
	}

private:
	zMutex mlock;
};

