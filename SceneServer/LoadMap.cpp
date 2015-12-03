#include "LoadMap.h"
#include <fstream>

using std::ios;

bool LoadMap(const std::string& filename, zTiles& aTiles, uint32_t& width, uint32_t& height)
{
	stMapFileHeader hdr;
	std::ifstream in(filename,ios::in|ios::binary);
	in.seekg(0, ios::end);
	const long filesize = in.tellg();
	if (filesize < sizeof(hdr)) 
		return false;
	in.seekg(0,ios::beg);
	in.read((char*)&hdr, sizeof(hdr));
	width = hdr.width;
	height = hdr.height;
	if (width * height * sizeof(stSrvMapTile) + sizeof(hdr) > filesize)
		return false;
	aTiles.resize(width * height);
	in.read((char*)&aTiles[0], aTiles.size() * sizeof(stSrvMapTile));
	return true;
}
