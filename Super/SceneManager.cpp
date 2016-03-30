#include "SceneManager.h"
#include "Scene.h"

void SceneManager::Add(std::shared_ptr<Scene> s)
{
	scenes[s->id] = s;
}
