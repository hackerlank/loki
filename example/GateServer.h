#pragma once

#include "server.h"
#include <iostream>
#include "logger.h"
#include <functional>
#include "mutex.h"
#include <unordered_map>
//#include "singleton.h"

using namespace std;
using namespace loki;


class GateServer 
{
public:
	GateServer(const std::string& ip,
			const std::string& port,
			io_service_pool& pool)
		:server_(ip, port, pool)
	{
		server_.set_msg_handler(
				std::bind(&GateServer::on_msg, this, std::placeholders::_1));
		server_.set_connected_handler(
				std::bind(&GateServer::on_connected, this,
					std::placeholders::_1));
		server_.start_accept();
	}

	void on_connected(connection_ptr conn)
	{
		std::cout<<"[main], on connected port="<<conn->get_peer_port()<<std::endl;

		{
			scoped_lock l(mutex_);
			connections_[conn->get_id()] = conn;
		}
	}

	void on_msg(connection_ptr conn)
	{
		//std::cout<<"got msg"<<std::endl;
		loki::buffer* buffer = conn->get_buffer();
		conn->send((const char*)buffer->data_ptr(), buffer->data_size());
		buffer->consume(buffer->data_size());
	}

	void on_error(connection_ptr conn)
	{
		if (conn->is_connected())
		{
			scoped_lock l(mutex_);
			connections_.erase(conn->get_id());
		}
	}
private:
	server server_;
	std::mutex mutex_;
	std::unordered_map<uint64_t, connection_ptr> connections_;
};

