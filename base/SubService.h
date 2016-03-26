#pragma once
#include "service.h"
#include "TcpServer.h"
#include "ServerList.h"
#include <memory>
#include <string>
#include "TcpConn.h"
#include "Proto.h"

namespace loki
{

class SubService : public service
{
private:
	static SubService* sub_instance_;
public:
	static SubService& get_instance() { return *sub_instance_; }

	SubService(io_service_pool& pool, const uint32_t server_type, const std::string& script_file);
	virtual ~SubService();
	virtual bool Init();


	void set_server_entry(const ServerEntryPtr& e) { myentry_ = e; }
	void add_entry(const ServerEntryPtr& e) { serverlist_.addEntry(e); }

	uint16_t get_port() const { return myentry_->port(); }
	std::string get_ip() const { return myentry_->ip(); } 
	uint32_t get_type() const { return type_; }
	uint32_t get_id() const { return myentry_->id(); }
	const std::string GetName() const { return myentry_->name(); }

	/*
	virtual void on_connected(connection_ptr conn) {}
	virtual void on_msg(connection_ptr conn);
	virtual void on_error(connection_ptr conn) {}

	void on_super_connected(connection_ptr conn);
	*/
	bool StartServer();
	virtual bool PostInit() { return true; }
	virtual void registerScript(lua_State* L) {}
	virtual void RegisterCallback() {}

	virtual void ExecuteMsg(TcpConnPtr, MessagePtr);
	virtual void HandleError(TcpConnPtr, const boost::system::error_code& );
	virtual void SyncHandleError(TcpConnPtr, const boost::system::error_code& ){}
protected:
	uint32_t type_ = 0;
	std::shared_ptr<TcpServer> server_;    //for client
	ServerList serverlist_;
	ServerEntryPtr myentry_;

	ProtoDispatcher superDispatcher_;
	ProtoDispatcher dispatcher_;

	void handleConnectSuper(TcpConnPtr conn, const boost::system::error_code& error);
	void disconnectSuper(TcpConnPtr conn, const boost::system::error_code& error);
public:
	TcpConnPtr superclient_;

private:
	void _registerScript(lua_State* L);
	std::string script_file_;
};

}
