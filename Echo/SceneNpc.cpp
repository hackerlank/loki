#include "SceneNpc.h"
#include "NpcData.h"
#include "Super.pb.h"
#include "Scene.h"

SceneNpc::SceneNpc(std::shared_ptr<NpcData> d, const uint32_t own)
{
	owner = own;
	data = new Super::stMapNpcData();
	data->set_npcid(d->id);
	data->set_type(d->type);
	data->set_name(d->name);
	data->set_maxhp(d->maxhp);
	data->set_hp(d->maxhp);
	data->set_damage(d->damage);
	data->set_level(1);
	data->set_attackrange(d->attackRange);
	data->set_attackinterval(d->attackInterval);
	data->set_tempid(tempid);
	data->set_accid(owner);
	data->set_movespeed(d->moveSpeed);

	memset(&lastAttack, 0, sizeof(lastAttack));
}

SceneNpc::~SceneNpc()
{
	delete data;
	LOG(INFO)<<__func__;
}

void SceneNpc::SendMeToNine()
{
	Super::stMapNpcData send;
	send = *data;
	scene->SendCmdToNine(&send);
}

void SceneNpc::MoveTo(const Super::Vector3& pos, float direction)
{
	data->mutable_position()->CopyFrom(pos);
	data->set_direction(direction);
	SendPositionToNine();
}

void SceneNpc::SendPositionToNineExcept(const uint32_t owner)
{
	Super::stNpcMoveCmd send;
	send.set_tempid(tempid);
	send.mutable_position()->CopyFrom(data->position());
	send.set_direction(data->direction());
	scene->SendCmdToNineExcept(&send, owner);
}

void SceneNpc::SendPositionToNine()
{
	Super::stNpcMoveCmd send;
	send.set_tempid(tempid);
	send.mutable_position()->CopyFrom(data->position());
	send.set_direction(data->direction());
	scene->SendCmdToNine(&send);
}

bool SceneNpc::CanMove()
{
	if (!data)
		return false;
	//building can not move
	if (data->type() == 10)
		return false;
	if (data->movespeed() == 0)
		return false;
	else
		return true;
}

bool SceneNpc::AttackOverSpeed()
{
	struct timeval cur;
	gettimeofday(&cur, nullptr);
	long dif = cur.tv_sec * 1000000 + cur.tv_usec - (lastAttack.tv_sec * 1000000 + lastAttack.tv_usec);
	return (dif >= static_cast<long>(data->attackinterval() * 1000));
}

void SceneNpc::ReduceHp(uint32_t change)
{
	if (change == 0)
		return;
	uint32_t realChange = 0;
	if (change >= data->hp())
	{
		realChange = data->hp();
		data->set_hp(0);
		dead = true;
	}
	else
	{
		realChange = change;
		data->set_hp(data->hp() - change);	//current data
	}
	Super::stHpCmd send;
	send.set_tempid(tempid);
	send.set_hp(data->hp());
	scene->SendCmdToNine(&send);
	LOG(INFO)<<"Change Hp = "<<realChange;
}
