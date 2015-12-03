#include "SceneServer.h"
#include "SceneTask.h"
#include <cstdlib>
#include "record.pb.h"
#include "session.pb.h"
#include "SceneCallback.h"
#include "SceneManager.h"
#include "ServerManager.h"

using namespace loki;

SceneServer::SceneServer(io_service_pool& pool, const std::string& script_file)
	:sub_service(pool, SCENESERVER, script_file)
{
}

void SceneServer::on_connected(connection_ptr conn)
{
	context_ptr task(new SceneTask(conn));
	conn->set_context(task);
	get_logic_service().post(boost::bind(&SceneServer::add_connection, this, conn));
}

void SceneServer::add_connection(connection_ptr conn)
{
	connections_[conn->get_id()] = conn;
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
}

void SceneServer::remove_connection(connection_ptr conn)
{
	context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));

	connections_.erase(conn->get_id());
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
	ServerManager::instance().erase(context->id());
}

void SceneServer::on_error(connection_ptr conn)
{
	get_logic_service().post(boost::bind(&SceneServer::remove_connection, this, conn));	
}

void SceneServer::registerCallback()
{
	using namespace tina;

	//from sub user
	dispatcher_.registerMsgCallback<SceneCmd::t_LoginScene>(std::bind(onLoginScene, std::placeholders::_1, std::placeholders::_2));

	dispatcher_.registerMsgCallback<Session::t_regScene_ret_SceneSession>(std::bind(onRegScene_ret_SceneSession, std::placeholders::_1, std::placeholders::_2));
}

bool SceneServer::start_server()
{
	new SceneManager();
	new ServerManager();

	recordclient_.reset(new connection(pool_.get_io_service()));
	recordclient_->set_msg_handler(std::bind(&SceneServer::on_msg, this, std::placeholders::_1));
	recordclient_->set_error_handler(std::bind(&SceneServer::handle_stop, this));			//TODO: change another handler
	ServerEntryPtr recordEntry = serverlist_.getEntryByType(RECORDSERVER);
	if (!recordEntry)
	{
		LOG(ERROR)<<("can't find record entry");
		return false;
	}
	if (!recordclient_->connect(recordEntry->ip(), recordEntry->port()))
	{
		LOG(ERROR)<<("can't find record entry");
		return false;
	}
	else
	{
		Record::t_LoginRecord send;
		send.set_id(myentry_->id());
		send.set_type(myentry_->type());
		codec_.send(recordclient_, &send);
	}

	sessionclient_.reset(new connection(pool_.get_io_service()));
	sessionclient_->set_msg_handler(std::bind(&SceneServer::on_msg, this, std::placeholders::_1));
	sessionclient_->set_error_handler(std::bind(&SceneServer::handle_stop, this));			//TODO: change another handler
	ServerEntryPtr sessionEntry = serverlist_.getEntryByType(SESSIONSERVER);
	if (!sessionEntry)
	{
		LOG(ERROR)<<("can't find session entry");
		return false;
	}
	if (!sessionclient_->connect(sessionEntry->ip(), sessionEntry->port()))
	{
		LOG(ERROR)<<("can't connect session entry");
		return false;
	}
	else
	{
		Session::t_LoginSession send;
		send.set_id(myentry_->id());
		send.set_type(myentry_->type());
		codec_.send(sessionclient_, &send);
	}
			
	LOG(INFO)<<("start server");

	started_ = true;

	/*
	if (!SceneManager::instance().init())
		return false;
		*/
	bool init_ret = lua_tinker::call<bool>(getLuaState(), "StartServer");
	return init_ret;
}

void SceneServer::registerScript(lua_State* L)
{
	Scene::registerScript(L);
	SceneManager::registerScript(L);
}
