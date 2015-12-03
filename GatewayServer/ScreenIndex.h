#pragma once
#include "zNoncopyable.h"
#include "type.h"
#include <set>
#include <unordered_map>
#include "zSceneEntry.h"
#include <map>
#include "zMutex.h"
#include "zEntryManager.h"
#include <google/protobuf/message.h>

class GatewayTask;

class ScreenIndex :private zNoncopyable
{
private:
	zRWLock wrlock;

	typedef std::set<GatewayTask*> SceneEntry_SET;
	typedef std::unordered_map<DWORD,SceneEntry_SET> PosIMapIndex;
	PosIMapIndex index;
	SceneEntry_SET all;

	const DWORD screenx;
	const DWORD screeny;
	const DWORD screenMax;
	typedef std::unordered_map<DWORD,zPosIVector> NineScreen_map;
	typedef NineScreen_map::iterator NineScreen_map_iter;
	typedef NineScreen_map::value_type NineScreen_map_value_type;
	NineScreen_map ninescreen;
	NineScreen_map direct_screen[8];
	NineScreen_map reversedirect_screen[8];
	inline const zPosIVector &getNineScreen(const zPosI &posi);
	inline const zPosIVector &getDirectScreen(const zPosI &posi,const int direct);
	inline const zPosIVector &getReverseDirectScreen(const zPosI &posi,const int direct);
public:
	ScreenIndex(const DWORD x,const DWORD y);
	template <class YourNpcEntry>
	void execAllOfScreen(const DWORD screen,execEntry<YourNpcEntry> &callback);
	void sendCmdToNine(const DWORD posi,const google::protobuf::Message* msg,unsigned short dupIndex = 0);
	void sendCmdToDirect(const zPosI posi,const int direct,const google::protobuf::Message* msg,unsigned short dupIndex = 0);
	void sendCmdToReverseDirect(const zPosI posi,const int direct,const google::protobuf::Message* msg,unsigned short dupIndex = 0);
	void sendCmdToAll(const google::protobuf::Message* msg);
	void sendCmdToNineExceptMe(const DWORD posi,const DWORD exceptme_id,const google::protobuf::Message* msg);
	bool refresh(GatewayTask *e,const DWORD newScreen);
	void removeGateUser(GatewayTask *e);
};

typedef std::map<DWORD,ScreenIndex*> MapIndex;
typedef MapIndex::iterator MapIndexIter;
