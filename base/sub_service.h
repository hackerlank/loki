/**
 * \file
 * \version  $Id: sub_service.h 0 2015-04-03 11:43:34 xxx $
 * \author  liuqing,liuqing1@ztgame.com
 * \date Fri Apr  3 11:43:34 2015
 * \brief sub service
 */

#pragma once
#include "service.h"
#include "server.h"
#include "ServerList.h"
#include <memory>
#include "protobuf_dispatcher.h"
#include "protobuf_codec.h"
#include <string>

namespace loki
{

class sub_service : public service
{
private:
	static sub_service* sub_instance_;
public:
	static sub_service& get_instance() { return *sub_instance_; }

	sub_service(io_service_pool& pool, const uint32_t server_type, const std::string& script_file);
	virtual ~sub_service();
	virtual bool init();


	void set_server_entry(const ServerEntryPtr& e) { myentry_ = e; }
	void add_entry(const ServerEntryPtr& e) { serverlist_.addEntry(e); }

	uint16_t get_port() const { return myentry_->port(); }
	std::string get_ip() const { return myentry_->ip(); } 
	uint32_t get_type() const { return type_; }
	uint32_t get_id() const { return myentry_->id(); }

	virtual void on_connected(connection_ptr conn) {}
	virtual void on_msg(connection_ptr conn);
	virtual void on_error(connection_ptr conn) {}

	void on_super_connected(connection_ptr conn);
	lua_State* getLuaState() { return script_->get_state(); }

	bool _start_server();
	virtual bool start_server() { return true; }
	virtual void registerCallback() {}
	virtual void registerScript(lua_State* L) {}
protected:
	uint32_t type_ = 0;
	std::shared_ptr<server> server_;    //for client
	ServerList serverlist_;
	ServerEntryPtr myentry_;

	protobuf_dispatcher dispatcher_;
	protobuf_codec codec_;

	void do_msg(connection_ptr conn, MessagePtr msg);
public:
	connection_ptr superclient_;

private:
	std::string script_file_;

	void _registerCallback();
	void _registerScript(lua_State* L);
};

}
