#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include "sub_service.h"
#include "singleton.h"
#include "context.h"

using loki::connection_ptr;
using loki::io_service_pool;
using loki::context_ptr;

class SessionServer : public loki::sub_service, public Singleton<SessionServer>
{
public:
	SessionServer(io_service_pool& pool, const std::string& file);

	//bool init();

	void on_connected(connection_ptr conn);
	void on_error(connection_ptr conn);

	bool start_server();
private:
	void registerCallback();
	std::unordered_map<uint64_t, connection_ptr> connections_;
	std::unordered_map<uint64_t, connection_ptr> work_conns_;
	
	std::unordered_map<uint32_t, context_ptr> sub_conns_; //all sub server connections

	void add_connection(connection_ptr conn);
	void remove_connection(connection_ptr conn);
public:
	connection_ptr recordclient_;
};

