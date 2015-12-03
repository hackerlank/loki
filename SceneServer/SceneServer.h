#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include "sub_service.h"
#include "singleton.h"
#include "context.h"

using loki::io_service_pool;
using loki::connection_ptr;
using loki::context_ptr;

class SceneServer : public loki::sub_service, public Singleton<SceneServer>
{
public:
	SceneServer(loki::io_service_pool& pool, const std::string& file);

	//bool init();

	void on_connected(connection_ptr conn);
	void on_error(connection_ptr conn);
	bool start_server();

	virtual void registerScript(lua_State* L);
private:
	void registerCallback();
	std::unordered_map<uint64_t, connection_ptr> connections_;
	std::unordered_map<uint64_t, connection_ptr> work_conns_;

private:
	void add_connection(connection_ptr conn);
	void remove_connection(connection_ptr conn);
private:

public:
	connection_ptr recordclient_;
	connection_ptr sessionclient_;
};

