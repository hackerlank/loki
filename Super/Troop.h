#pragma once

#include <map>
#include <vector>
#include <memory>

class NpcData;

class Troop
{
public:
	std::map<uint32_t, std::shared_ptr<NpcData> > datas;

	void Add(std::shared_ptr<NpcData> data);

	void Remove(const uint32_t id);
};
