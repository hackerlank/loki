#pragma once

#include "SceneObject.h"
#include <memory>
#include "Vector.h"
#include "Super.pb.h"

class NpcData;

class SceneNpc : public SceneObject
{
	public:
		SceneNpc(std::shared_ptr<NpcData> data, const uint32_t own);
		~SceneNpc();

		uint32_t owner;

		virtual void SendMeToNine();

		Super::stMapNpcData* data;

		void MoveTo(const Super::Vector3& pos, float direction);

		void SendPositionToNine();
};
