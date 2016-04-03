#pragma once

#include "singleton.h"
#include <map>
#include <memory>

class Scene;

class SceneManager : public Singleton<SceneManager>
{
private:
	std::map<uint32_t, Scene*> scenes;

	uint32_t leftTime;
public:
	void Add(Scene* s);

	void Update(long delta);
};
