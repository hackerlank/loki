#include "SceneNpc.h"
#include "SceneDefine.h"

SceneNpc::~SceneNpc()
{
}

SceneNpc::SceneNpc(Scene* s, zNpcB* n, const t_NpcDefine* d, const SceneNpcType type)
	:SceneEntryPk(s, SceneEntry_NPC, SceneEntry_Hide), npc(n), define(d)
{
}

bool SceneNpc::moveable(const zPos& tempPos, const zPos& destPos, const int radius) const
{
	return (scene->zPosShortRange(tempPos, destPos, radius) 
			&& (!scene->checkBlock(tempPos) ||tempPos== destPos));
}
