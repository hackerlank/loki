#pragma once

#include "singleton.h"
#include <map>
#include <memory>

template<class T>
class MapManager
{
public:
	std::map<uint32_t, T> data;

	void Add(uint32_t key, const T& value)
	{
		data[key] = value;
	}
	void Remove(uint32_t key)
	{
		data.erase(key);
	}

	T Get(uint32_t key)
	{
		return data[key];
	}
	bool ContainKey(uint32_t key)
	{
		auto it = data.find(key);
		return it != data.end();
	}

};
