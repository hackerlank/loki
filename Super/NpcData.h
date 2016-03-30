#pragma once

#include "NpcBase.h"

class NpcData
{
public:
	const NpcBase* base;
	uint32_t id = 0;
	std::string name;
	uint32_t type = 0;
	//uint32_t hp = 0;
	uint32_t maxhp = 0;
	uint32_t damage = 0;
	float attackRange = 0;
	float attackInterval = 0;
	float moveSpeed = 0;
};
