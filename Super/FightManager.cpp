#include "FightManager.h"
#include <string>
#include "Scene.h"
#include "SceneManager.h"
#include "Super.pb.h"

void FightManager::PrepareFight(PlayerEntity* a, PlayerEntity* b)
{
	LOG(INFO)<<"Prepare a fight for ("<<a->name<<","<<b->name<<")";
	std::shared_ptr<Scene> scene(new Scene());
	std::string filename = "test.map";
	if (!scene->LoadMap(filename))
	{
		LOG(INFO)<<"Load map "<<filename<<" error";
		return ;
	}
	scene->AddPlayer(a);
	scene->AddPlayer(b);
	SceneManager::instance().Add(scene);
	Super::stEnterFightScene send;
	send.set_scenename("FightScene");
	send.set_host(1);
	a->SendCmd(&send);
	send.set_host(0);
	b->SendCmd(&send);
}

void FightManager::Update(long delta)
{
	if (data.size() >= 2)
	{
		LOG(INFO)<<"Create a battle";
		auto it = data.begin();
		auto a = it->second;
		it = data.erase(it);
		auto b =it->second;
		data.erase(it);
		PrepareFight(a, b);
	}
}
