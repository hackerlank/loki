#pragma once

#include "SceneObject.h"
#include <memory>
#include "Vector.h"

class PlayerEntity;

class SceneTotem : public SceneObject
{
	public:
		SceneTotem(PlayerEntity* player);
		~SceneTotem();

		uint32_t owner;

		virtual void SendMeToNine();
		uint32_t hp;
		uint32_t maxhp;
};
