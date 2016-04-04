#include "Scene.h"
#include "PlayerEntity.h"
#include <cassert>
#include "SceneNpc.h"
#include "Super.pb.h"
#include "PlayerManager.h"

uint32_t Scene::s_id = 0;

Scene::Scene():id(++s_id)
{
	overTime = time(nullptr) + 180;
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
	assert(a->scene == nullptr);
	assert(players.size() <= 1);
	players.push_back(a);
	
	ChangeToScene(a);
	return true;
}

void Scene::ChangeToScene(PlayerEntity* a)
{
	a->scene = this;
	//Change to Fight Scene
	Super::stEnterFightScene send;
	send.set_host(a->host);
	send.set_scenename(sceneName);
	a->SendCmd(&send);
}

void Scene::RemoveSceneNpc(const uint32_t tempid)
{
	Super::stRemoveSceneNpc send;
	send.set_tempid(tempid);
	SendCmdToNine(&send);
}

void Scene::RemovePlayer(PlayerEntity* player)
{
	if (this != player->scene)
	{
		return ;
	}
	player->scene = nullptr;
	if (!player->IsOnline())
	{
		PlayerManager::instance().Remove(player->accid);
		delete player;
	}
}

void Scene::AddSceneNpc(SceneNpc* obj, bool notify)
{
	obj->scene = this;
	objs[obj->tempid] = obj;
	LOG(INFO)<<__func__<<", tempid="<<obj->tempid;

	if (notify)
		obj->SendMeToNine();
}

void Scene::SendCmdToNine(const google::protobuf::Message* msg)
{
	for (int i = 0; i != players.size(); ++i)
	{
		if (players[i]->IsOnline())
			players[i]->SendCmd(msg);
	}
}

void Scene::SendCmdToNine(const loki::MessagePtr msg)
{
	SendCmdToNine(msg.get());
}

//创建玩家基地
void Scene::CreatePlayerBase()
{
	float zPos = 40;
	for(int i=0; i<players.size(); ++i)
	{
		auto p = players[i];
		SceneNpc* npc(new SceneNpc(p->base, p->accid));
		if (i == 0)
		{
			npc->data->mutable_position()->set_x(0);
			npc->data->mutable_position()->set_y(0);
			npc->data->mutable_position()->set_z(-zPos);
			npc->data->set_direction(0);
		}
		else if (i == 1)
		{
			npc->data->mutable_position()->set_x(0);
			npc->data->mutable_position()->set_y(0);
			npc->data->mutable_position()->set_z(zPos);
			npc->data->set_direction(180);
		}
		base[i] = npc;
		AddSceneNpc(npc, false);
	}
}

void Scene::SendBaseInfoToUser(PlayerEntity* player)
{
	for (int i=0;i<2;++i)
	{
		player->SendCmd(base[i]->data);
	}
}

void Scene::Prepare()
{
	CreatePlayerBase();
}

void Scene::Leave(PlayerEntity* player)
{
}

void Scene::Update(long delta)
{
	if (deleteMe)
		return;
	bool del = true;
	for(int i=0;i< players.size(); ++i)
	{
		if (players[i]->IsOnline())
		{
			del = false;
			break;
		}
	}
	deleteMe = del;
	if (deleteMe)
	{
		LOG(INFO)<<"Destroy Scene: no player in scene id = "<<id;
	}
	if (time(nullptr) > overTime)
	{
		deleteMe = true;
		LOG(INFO)<<"Destroy Scene: OverTime is reach id = "<<id;
	}
	if (deleteMe)
	{
		Destroy();
	}
}


void Scene::Destroy()
{
	//delete everyting in the scene, if there is no player online
	for (auto it = objs.begin(); it != objs.end(); ++it)
	{
		auto npc = it->second;
		delete npc;
	}

	for(int i=0;i< players.size(); ++i)
	{
		RemovePlayer(players[i]);
	}
	players.clear();
}

SceneNpc* Scene::GetSceneNpcByTempid(uint32_t tempid)
{
	auto it = objs.find(tempid);
	if (it == objs.end())
	{
		return nullptr;
	}
	else
	{
		return it->second;
	}
}
