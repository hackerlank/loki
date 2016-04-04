#pragma once

#include <vector>
#include <map>
#include <memory>
#include "TcpConn.h"
#include <set>

class PlayerEntity;
class SceneNpc;

class Scene
{
private:
	float width;
	float height;
	static uint32_t s_id;
public:
	uint32_t id;
	std::string filename;
	std::string sceneName;
	Scene();
	bool LoadMap(const std::string& filename);

	bool AddPlayer(PlayerEntity* a);
	void Leave(PlayerEntity* a);

	std::vector<PlayerEntity*> players;

	SceneNpc* base[2];
	std::map<uint32_t, SceneNpc*> objs;

	SceneNpc* GetSceneNpcByTempid(uint32_t tempid);

	void AddSceneNpc(SceneNpc* obj, bool notify = true);
	//void RemoveSceneNpcByOwner(PlayerEntity* );
	void SendCmdToNine(const loki::MessagePtr msg);
	void SendCmdToNine(const google::protobuf::Message* msg);

	void CreatePlayerBase();
	void Prepare();

	void SendBaseInfoToUser(PlayerEntity* );

	void RemoveSceneNpc(const uint32_t tempid);
	void RemovePlayer(PlayerEntity*);

	bool deleteMe = false;

	time_t overTime = 0;
	void Update(long delta);
	void Destroy();
};
