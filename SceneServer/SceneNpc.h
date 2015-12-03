#pragma once

#include "SceneEntryPk.h"
#include "zAStar.h"
#include "Scene.h"
#include "zDatabase.h"

enum SceneNpcType
{
	STATIC,
	GANG
};

struct t_NpcDefine;

class SceneNpc: public SceneEntryPk//, public zAStar<>, public zAStar<2>
{
public:
	SceneNpc(Scene* scene, zNpcB* npc, const t_NpcDefine* define, const SceneNpcType type);

	virtual ~SceneNpc();
	bool isFunctionNpc() const { return true; }
	bool moveable(const zPos& tempPos, const zPos& destPos, const int radius) const;

	zNpcB* npc;
	const t_NpcDefine* define;
};
