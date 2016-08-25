#pragma once
#include <map>
#include "singleton.h"
#include <ctime>
#include <memory>
#include <cassert>
#include "PlayerEntity.h"

class PlayerEntity;

class FightManager: public Singleton<FightManager>
{
public:
	std::map<uint32_t, PlayerEntity*> data;
public:
	void Add(const uint32_t id, PlayerEntity* player)
	{
		assert(player);
		data[id] = player;
	}

	void Remove(const uint32_t id)
	{
		data.erase(id);
	}

	PlayerEntity* Get(const uint32_t acc)
	{
		auto it = data.find(acc);
		if (it != data.end())
			return it->second;
		else
			return nullptr;
	}
	
	void Update(long delta);

	void PrepareFight(PlayerEntity* a, PlayerEntity* b);
};
