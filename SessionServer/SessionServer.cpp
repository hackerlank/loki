#include "SessionServer.h"
#include "SessionTask.h"
#include <cstdlib>
#include "record.pb.h"
#include "session.pb.h"
#include "SessionCallback.h"
#include "SceneSessionManager.h"

using namespace loki;

SessionServer::SessionServer(io_service_pool& pool, const std::string& script_file)
	:sub_service(pool, SESSIONSERVER, script_file)
{
}

void SessionServer::on_connected(connection_ptr conn)
{
	context_ptr task(new SessionTask(conn));
	conn->set_context(task);
	get_logic_service().post(boost::bind(&SessionServer::add_connection, this, conn));
}

void SessionServer::add_connection(connection_ptr conn)
{
	connections_[conn->get_id()] = conn;
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
}

void SessionServer::remove_connection(connection_ptr conn)
{
	context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
	sub_conns_.erase(context->id());
	connections_.erase(conn->get_id());
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
}

void SessionServer::on_error(connection_ptr conn)
{
	get_logic_service().post(boost::bind(&SessionServer::remove_connection, this, conn));	
}

void SessionServer::registerCallback()
{
	using namespace Session;
	//from sub user
	dispatcher_.registerMsgCallback<Session::t_LoginSession>(std::bind(onLogin, std::placeholders::_1, std::placeholders::_2));

	dispatcher_.registerMsgCallback<Session::t_regScene_SceneSession>(std::bind(onRegScene_SceneSession, std::placeholders::_1, std::placeholders::_2));
}

bool SessionServer::start_server()
{
	new SceneSessionManager();

	recordclient_.reset(new connection(pool_.get_io_service()));
	recordclient_->set_msg_handler(std::bind(&SessionServer::on_msg, this, std::placeholders::_1));
	recordclient_->set_error_handler(std::bind(&SessionServer::handle_stop, this));			//TODO: change another handler
	ServerEntryPtr recordEntry = serverlist_.getEntryByType(RECORDSERVER);
	if (!recordEntry)
	{
		LOG(INFO)<<("can't find record entry");
		return false;
	}
	if (!recordclient_->connect(recordEntry->ip(), recordEntry->port()))
	{
		LOG(INFO)<<("can't connect record entry");
		return false;
	}
	else
	{
		Record::t_LoginRecord send;
		send.set_id(myentry_->id());
		send.set_type(myentry_->type());
		codec_.send(recordclient_, &send);
	}
	LOG(INFO)<<("start server");
	started_ = true;
	return true;
}

