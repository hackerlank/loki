#pragma once

#include "server.h"
#include <iostream>
#include "logger.h"
#include <functional>
#include <unordered_map>
#include "script.h"
#include "Service.h"
#include "protobuf_dispatcher.h"
#include "protobuf_codec.h"
#include "singleton.h"
#include "ServerList.h"
#include "context.h"

using namespace std;
using namespace loki;


class SuperServer : public service, public Singleton<SuperServer>
{
public:
	SuperServer(const std::string& file, io_service_pool& pool);

	bool init();

	void on_connected(connection_ptr conn);
	void on_login_connected(connection_ptr conn);
	void on_msg(connection_ptr conn);
	void on_error(connection_ptr conn);

	void init_thread_data();

private:
	protobuf_dispatcher dispatcher_;
	protobuf_codec codec_;
private:
	void registerCallback();
	std::unordered_map<uint64_t, connection_ptr> connections_;	//all connections
	std::unordered_map<uint64_t, connection_ptr> work_conns_;

	//for client user
	std::shared_ptr<server> server_;	//for client

	std::string script_file_;	//login.lua
	std::shared_ptr<script> script_;

	uint16_t port_{0};
	
private:
	void do_msg(connection_ptr conn, MessagePtr msg);

	void add_connection(connection_ptr conn);
	void remove_connection(connection_ptr conn);

	bool checkZoneConn(connection_ptr conn, const uint32_t type);
private:
	void loginFL(connection_ptr conn);

	uint32_t game_ = 0;
	uint32_t zone_ = 0;
	std::string name_;

	std::map<int, std::vector<int> > serverSequence; //type->types
	//服务器之间的依赖
	std::map<int, std::vector<int> > dependency_;       //type->ids
public:
	connection_ptr flclient_;

public:
	void set_game(const uint32_t game) { game_ = game; }
	void set_zone(const uint32_t zone) { zone_ = zone; }
	void set_name(const std::string& name) { name_ = name; }
	std::unordered_map<uint32_t, context_ptr> sub_conns_;	//all sub server connections
	ServerList serverlist_;

	ServerList& get_serverlist() { return serverlist_; }
	void LoadServerList();
	std::vector<int> getDependencyID(const int type) const;

};

