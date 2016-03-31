#include "Scene.h"
#include "PlayerEntity.h"
#include <cassert>
#include "SceneObject.h"
#include "Super.pb.h"
#include "SceneTotem.h"

uint32_t Scene::s_id = 0;

Scene::Scene():id(++s_id)
{
	player[0] = player[1] = nullptr;
}

bool Scene::LoadMap(const std::string& filename)
{
	this->filename = filename;
	sceneName = "FightScene";
	width = 100;
	height = 200;
	return true;
}

bool Scene::AddPlayer(PlayerEntity* a)
{
	uint32_t host = 0;
	if (player[0] == nullptr)
	{
		player[0] = a;
		host = 1;
	}
	else if (player[1] == nullptr)
	{
		player[1] = a;
		host = 0;
	}
	else
	{
		return false;
	}
	a->scene = shared_from_this();
	//Change to Fight Scene
	Super::stEnterFightScene send;
	send.set_host(host);
	send.set_scenename(sceneName);
	a->SendCmd(&send);
	return true;
}

void Scene::AddSceneObject(std::shared_ptr<SceneObject>& obj)
{
	objs[obj->tempid] = obj;
	LOG(INFO)<<__func__<<", tempid="<<obj->tempid;
}

void Scene::SendCmdToNine(const google::protobuf::Message* msg)
{
	for(int i=0;i<2;++i)
	{
		if (player[i])
			player[i]->SendCmd(msg);
	}
}

void Scene::SendCmdToNine(const loki::MessagePtr msg)
{
	SendCmdToNine(msg.get());
}

//创建玩家基地
void Scene::CreatePlayerBase()
{
	float yPos = 10;
	for(int i=0;i<2;++i)
	{
		if (player[i])
		{
			std::shared_ptr<SceneObject> npc(new SceneTotem(player[i]));
			if (i == 0)
			{
				npc->position = Vector3(0, -yPos, 0);
				npc->dir = 0;
			}
			else
			{
				npc->position = Vector3(0, yPos, 0);
				npc->dir = 180;
			}
			base[i] = npc;
		}
	}
}

void Scene::SendBaseInfoToUser(PlayerEntity* player)
{
	for (int i=0;i<2;++i)
	{
		//player->SendCmd();
	}
}

void Scene::Prepare()
{
	CreatePlayerBase();
}

void Scene::Leave(PlayerEntity* player)
{
}
