#pragma once

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "lua_tinker.h"

#include <boost/noncopyable.hpp>
#include <string>

namespace loki
{

class script : private boost::noncopyable
{
public:
	script()
	{
		L = luaL_newstate();
		luaL_openlibs(L);
	}

	~script()
	{
		if (L)
			lua_close(L);
	}
	bool dofile(const std::string& file)
	{
		lua_tinker::dofile(L, file.c_str());
		return true;
	}
	lua_State* get_state() { return L; }
	lua_State* GetLuaState() { return L; }

	template<class T>
		T get(const char* name)
		{
			return lua_tinker::get<T>(L, name);
		}
private:
	lua_State* L;
};

}
