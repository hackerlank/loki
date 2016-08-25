#pragma once

#include <map>
#include <vector>
#include <memory>
#include "MapManager.h"

class NpcData;

class Troop : public MapManager<std::shared_ptr<NpcData> >
{
public:
};
