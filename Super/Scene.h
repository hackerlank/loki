#pragma once

#include <vector>
#include <map>
#include <memory>
#include "TcpConn.h"

class PlayerEntity;
class SceneObject;

class Scene : public std::enable_shared_from_this<Scene>
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

	PlayerEntity* player[2];
	std::shared_ptr<SceneObject> base[2];

	std::map<uint32_t, std::shared_ptr<SceneObject> > objs;

	void AddSceneObject(std::shared_ptr<SceneObject>& obj);
	void SendCmdToNine(const loki::MessagePtr msg);
	void SendCmdToNine(const google::protobuf::Message* msg);

	void CreatePlayerBase();
	void Prepare();

	void SendBaseInfoToUser(PlayerEntity* );
};
