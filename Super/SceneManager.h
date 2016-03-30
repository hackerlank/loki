#pragma once

#include "singleton.h"
#include <map>
#include <memory>

class Scene;

class SceneManager : public Singleton<SceneManager>
{
private:
	std::map<uint32_t, std::shared_ptr<Scene> > scenes;

public:
	void Add(std::shared_ptr<Scene> s);
};
