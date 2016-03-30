#include "Troop.h"
#include "NpcData.h"

void Troop::Add(std::shared_ptr<NpcData> data)
{
	datas[data->id] = data;
}

void Troop::Remove(const uint32_t id)
{
	datas.erase(id);
}
