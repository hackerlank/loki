#pragma once

#include "SceneObject.h"
#include <memory>
#include "Vector.h"

class NpcData;

class SceneNpc : public SceneObject
{
	public:
		SceneNpc(std::shared_ptr<NpcData> data);
		~SceneNpc();
};
