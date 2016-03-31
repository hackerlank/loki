#include "SceneTotem.h"
#include "Super.pb.h"
#include "Scene.h"
#include "PlayerEntity.h"

SceneTotem::SceneTotem(PlayerEntity* player)
{
	owner = player->accid;
	hp = 100;
	maxhp = hp;
}

SceneTotem::~SceneTotem()
{
}

void SceneTotem::SendMeToNine()
{
}
