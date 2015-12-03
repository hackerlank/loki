#pragma once

#include <boost/unordered_map.hpp>

#pragma pack(1)

struct GameZone_t
{
	union {
		uint32_t id = 0;
		struct {
			uint16_t game;
			uint16_t zone;
		};
	};
	bool operator == (const GameZone_t& g) const
	{
		return (this->id == g.id);
	}
};

namespace std
{

template<>
struct hash<GameZone_t>
{
	size_t operator()(const GameZone_t& gameZone) const
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, gameZone.id);		//TODO: use std , not boost
		return seed;
	}
};

}

#pragma pack()
