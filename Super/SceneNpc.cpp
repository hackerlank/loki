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
}

SceneNpc::~SceneNpc()
{
	delete data;
}

void SceneNpc::SendMeToNine()
{
	Super::stMapNpcData send;
	send = *data;
	scene->SendCmdToNine(&send);
}
