#include "PlayerEntity.h"
#include "Scene.h"
#include "NpcData.h"
#include "SceneNpc.h"
#include "FightManager.h"
#include "PlayerManager.h"

using namespace loki;

PlayerEntity::PlayerEntity(TcpConnPtr conn): ConnEntity(conn),
	searchFight(false)
{
	conn->SetData(this);
	{
		base.reset(new NpcData());
		auto npc = base;
		npc->id = 1;
		npc->level = 1;
		npc->name = "aaa";
		npc->type = 10;
		npc->maxhp = 200;
		npc->damage = 10;
		npc->attackRange = 1.2f;
		npc->attackInterval = 1.2f;
		npc->moveSpeed = 0;
	}

	{
		std::shared_ptr<NpcData> npc(new NpcData());
		npc->id = 1;
		npc->level = 1;
		npc->name = "aaa";
		npc->type = 1;
		npc->maxhp = 200;
		npc->damage = 10;
		npc->attackRange = 1.2f;
		npc->attackInterval = 1.2f;
		npc->moveSpeed = 640;
		troop.Add(npc->id, npc);
	}

	{
		std::shared_ptr<NpcData> npc(new NpcData());
		npc->id = 2;
		npc->level = 2;
		npc->name = "bbb";
		npc->type = 1;
		npc->maxhp = 200;
		npc->damage = 10;
		npc->attackRange = 1.2f;
		npc->attackInterval = 1.2f;
		npc->moveSpeed = 640;
		troop.Add(npc->id, npc);
	}
}

PlayerEntity::~PlayerEntity()
{
	LOG(INFO)<<__func__;
}

void PlayerEntity::EnterScene(Scene* scene)
{
	for(auto it = troop.data.begin(); it != troop.data.end(); ++it)
	{
		SceneNpc* npc(new SceneNpc(it->second, accid));
		scene->AddSceneNpc(npc);
	}
}

void PlayerEntity::SendCardToMe()
{
	//拥有的卡片
	Super::stPlayerAllCards send;
	for(auto it = troop.data.begin(); it != troop.data.end(); ++it)
	{
		Super::stCardInfo* card = send.add_card();
		card->set_id(it->second->id);
		card->set_level(it->second->level);
	}
	SendCmd(&send);
}

void PlayerEntity::DispatchCard(std::shared_ptr<Super::stDispatchCard> msg)
{
	if (!troop.ContainKey(msg->id()))
	{
		LOG(INFO)<<"Error id="<<msg->id();
		return ;
	}
	if (dispatched.find(msg->id())!= dispatched.end())
	{
		LOG(INFO)<<"Already dispatched id="<<msg->id();
		return ;
	}
	auto npcdata = troop.Get(msg->id());
	SceneNpc* npc(new SceneNpc(npcdata, accid));
	//TODO: check position and dir validation
	npc->data->mutable_position()->CopyFrom(msg->position());
	float dir = host? 0: 180;
	npc->data->set_direction(dir);
	scene->AddSceneNpc(npc);

	dispatched.insert(msg->id());
}

void PlayerEntity::SetOnline(bool on)
{
	online = on;
	LOG(INFO)<<"Player Online = "<<online;
}

void PlayerEntity::Offline()
{
	LOG(INFO)<<"User offline name = "<<name;
	SetOnline(false);
	FightManager::instance().Remove(accid);
	//PlayerManager::instance().Remove(accid);
	relogin = false;
}

void PlayerEntity::Login()
{
	LOG(INFO)<<__func__;
	SetOnline(true);

	Super::stLoginGameServerResult send;
	send.set_ret(0);
	SendCmd(&send);

	PlayerManager::instance().Add(accid, this);

	SendMainData();
}

void PlayerEntity::SendMainData()
{
	Super::stPlayerData ss;
	ss.set_accid(accid);
	ss.set_name(name);
	SendCmd(&ss);
}

void PlayerEntity::Relogin()
{
	LOG(INFO)<<__func__;

	SetOnline(true);

	Super::stLoginGameServerResult send;
	send.set_ret(0);
	SendCmd(&send);

	SendMainData();

	if (scene)
	{
		scene->ChangeToScene(this);
	}
	relogin = true;
}

void PlayerEntity::NpcMove(std::shared_ptr<Super::stNpcMoveCmd> msg)
{
	SceneNpc* npc = scene->GetSceneNpcByTempid(msg->tempid());
	if (npc == nullptr)
	{
		LOG(INFO)<<"Unknown npc tempid="<<msg->tempid();
		return ;
	}
	if (npc->owner != accid)
	{
		LOG(INFO)<<"Can't move npc tempid="<<msg->tempid()<<", is not mine";
		return ;
	}
	//TODO: check distance and speed and msg frequence
	if (npc->CanMove())
		npc->MoveTo(msg->position(), msg->direction());
}

void PlayerEntity::EnterFightScene()
{
	if (scene == nullptr)
		return;
	//卡片信息
	SendCardToMe();
	//场景上的npc信息
	scene->SendNpcToPlayer(this);

	if (!relogin)
	{
	}
	else
	{
	}
}
