#pragma once

#include "gamezone.h"
#include <string>

struct ACLZone
{
	GameZone_t gamezone;
	std::string ip;
	std::string name;
	std::string desc;
	uint16_t port = 0;
};
