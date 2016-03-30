#include "Scene.h"
#include "PlayerEntity.h"
#include <cassert>
#include "SceneObject.h"

uint32_t Scene::s_id = 0;

Scene::Scene():id(++s_id)
{
	host = guest = nullptr;
}

bool Scene::LoadMap(const std::string& filename)
{
	this->filename = filename;
	width = 1024;
	height = 1920;
	return true;
}

bool Scene::AddPlayer(PlayerEntity* a)
{
	if (host == nullptr)
	{
		host = a;
	}
	else if (guest == nullptr)
	{
		guest = a;
	}
	else
	{
		assert(false);
		return false;
	}
	a->EnterScene(shared_from_this());
	return true;
}

void Scene::AddSceneObject(std::shared_ptr<SceneObject>& obj)
{
	objs[obj->tempid] = obj;
}
