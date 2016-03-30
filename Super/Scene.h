#pragma once

#include <vector>
#include <map>
#include <memory>

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
	Scene();
	bool LoadMap(const std::string& filename);

	bool AddPlayer(PlayerEntity* a);

	PlayerEntity* host;
	PlayerEntity* guest;

	std::map<uint32_t, std::shared_ptr<SceneObject> > objs;

	void AddSceneObject(std::shared_ptr<SceneObject>& obj);
};
