#pragma once

#include "server.h"
#include <iostream>
#include <functional>
#include <unordered_map>
#include "script.h"
#include "service.h"
#include "singleton.h"
#include "Login.pb.h"
#include <mutex>
#include "protobuf_dispatcher.h"
#include "protobuf_codec.h"
#include "gamezone.h"
#include "ACLZone.h"

using loki::service;
using loki::connection_ptr;
using loki::script;
using loki::server;
using loki::MessagePtr;
using loki::protobuf_dispatcher;
using loki::protobuf_codec;

class LoginServer : public service, public Singleton<LoginServer>
{
public:
	LoginServer(const std::string& file, loki::io_service_pool& pool);

	bool init();

	//zone server
	void on_server_connected(connection_ptr conn);
	void on_msg(connection_ptr conn);
	void on_error(connection_ptr conn);


	//client
	void on_user_connected(connection_ptr conn);
	void on_user_msg(connection_ptr conn);
	void on_user_error(connection_ptr conn);

	void init_thread_data();
private:
	protobuf_dispatcher dispatcher_;
	protobuf_codec codec_;
private:
	void registerCallback();
	//for zone
	std::shared_ptr<server> zone_server_;	//for zone server
	std::unordered_map<uint64_t, connection_ptr> user_conns_;

	//for client user
	std::shared_ptr<server> login_server_;	//for client

	std::string script_file_;	//login.lua
	std::shared_ptr<script> script_;
	
private:
	void do_msg(connection_ptr conn, MessagePtr msg);

private:
	std::unordered_map<uint64_t, connection_ptr> work_conns_;

	std::mutex mutex_;
	std::unordered_map<GameZone_t, std::shared_ptr<ACLZone> > aclmap;
	bool loadAcl();
public:
	void add_user_connection(connection_ptr conn);
	void remove_user_connection(connection_ptr conn);

	std::shared_ptr<ACLZone> getAcl(const std::string& ip, const uint16_t port);
	bool broadcastByID(const uint64_t conn_id, const MessagePtr& msg);
	bool broadcastByID(const uint64_t conn_id, const google::protobuf::Message* msg);
};

