#pragma once

#include "SceneObject.h"
#include <memory>
#include "Vector.h"
#include "Super.pb.h"
#include<time.h>

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
		void SendPositionToNineExcept(const uint32_t accid);

		bool CanMove();
		struct timeval lastAttack;

		bool AttackOverSpeed();

		bool IsAlive() { return data->hp() > 0; }
		//void AttackMe(SceneNpc* attacker, uint32_t id);
		void ReduceHp(uint32_t change);
		bool dead = false;
};
