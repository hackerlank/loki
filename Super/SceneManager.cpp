#include "SceneManager.h"
#include "Scene.h"

void SceneManager::Add(Scene* s)
{
	scenes[s->id] = s;
}

void SceneManager::Update(long delta)
{
	std::vector<Scene*> del;
	for(auto it = scenes.begin(); it != scenes.end(); ++it)
	{
		auto scene = it->second;
		if (scene->deleteMe)
			del.push_back(scene);
		else
			scene->Update(delta);
	}
	for (int i=0;i<del.size(); ++i)
	{
		auto scene = del[i];
		scenes.erase(scene->id);
		LOG(INFO)<<"Delete scene tempid = "<<scene->id;
		delete scene;
	}
}
