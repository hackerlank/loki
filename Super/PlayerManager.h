#pragma once

#include "singleton.h"
#include <map>
#include <memory>
#include "MapManager.h"

class PlayerEntity;

class PlayerManager: public Singleton<PlayerManager>, public MapManager<PlayerEntity*>
{
public:
};
