#include "PlayerEntity.h"
#include "Scene.h"
#include "NpcData.h"
#include "SceneNpc.h"

using namespace loki;

PlayerEntity::PlayerEntity(TcpConnPtr conn): ConnEntity(conn),
	searchFight(false)
{
	std::shared_ptr<NpcData> npc(new NpcData());
	npc->id = 1;
	npc->name = "aaa";
	npc->type = 1;
	npc->maxhp = 200;
	npc->damage = 10;
	npc->attackRange = 1.2f;
	npc->attackInterval = 1.2f;
	npc->moveSpeed = 3;
	troop.Add(npc);

	npc->id = 2;
	npc->name = "bbb";
	npc->type = 2;
	npc->maxhp = 200;
	npc->damage = 10;
	npc->attackRange = 3.2f;
	npc->attackInterval = 1.2f;
	npc->moveSpeed = 3;
	troop.Add(npc);

	npc->id = 3;
	npc->name = "ccc";
	npc->type = 3;
	npc->maxhp = 200;
	npc->damage = 10;
	npc->attackRange = 2.2f;
	npc->attackInterval = 1.2f;
	npc->moveSpeed = 3;
	troop.Add(npc);
}

PlayerEntity::~PlayerEntity()
{
	LOG(INFO)<<__func__;
}

void PlayerEntity::EnterScene(std::shared_ptr<Scene> scene)
{
	/*
	for(auto it = troop.datas.begin(); it != troop.datas.end(); ++it)
	{
		std::shared_ptr<SceneObject> npc(new SceneNpc(it->second));
		scene->AddSceneObject(npc);
	}
	*/
}
