#pragma once
#include <string>
#include <cstddef>
#include <vector>

#pragma pack(1)

struct stMapFileHeader
{
	uint32_t magic;
	uint32_t ver;
	uint32_t width;
	uint32_t height;
};

struct stSrvMapTile
{
	uint8_t flags;		//格子属性
	uint8_t type;		//格子类型
};
#pragma pack()

typedef stSrvMapTile Tile;
typedef std::vector<stSrvMapTile> zTiles;

extern bool LoadMap(const std::string& filename, zTiles& atiles, uint32_t &width, uint32_t &height);

enum TileType
{
	TILE_BLOCK = 0x01,
	TILE_MAGIC_BLOCK = 0x02,
	TILE_WATER_BLOCK = 0x04,
	TILE_DOOR = 0x08,
	TILE_ENTRY_BLOCK = 0x10,
	TILE_OBJECT_BLOCK = 0x20,
};
