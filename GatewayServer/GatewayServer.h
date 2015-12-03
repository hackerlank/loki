#pragma once

#include <iostream>
#include "logger.h"
#include <functional>
#include <unordered_map>
#include "script.h"
#include "sub_service.h"
#include "context.h"
#include "singleton.h"

using loki::connection_ptr;

class GatewayServer : public loki::sub_service, public Singleton<GatewayServer>
{
public:
	GatewayServer(loki::io_service_pool& pool, const std::string& file);

	//virtual bool init();
	virtual bool start_server();

	void on_connected(connection_ptr conn);
	void on_error(connection_ptr conn);

	void init_thread_data();

private:
	void registerCallback();
	std::unordered_map<uint64_t, connection_ptr> connections_;
	std::unordered_map<uint64_t, connection_ptr> work_conns_;

	std::unordered_map<uint32_t, loki::context_ptr> sub_conns_;

	boost::asio::deadline_timer one_second_timer_;
	void one_second_callback(const boost::system::error_code& e);
private:
	void add_connection(connection_ptr conn);
	void remove_connection(connection_ptr conn);
public:
	connection_ptr recordclient_;
	connection_ptr sessionclient_;
	//std::map<uint32_t , connection_ptr> sceneclients_;
	size_t get_users_num() const;
	void notify_login_server();
	void handle_stop();
};

