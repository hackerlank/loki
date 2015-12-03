#include "SuperServer.h"
#include "ServerTask.h"
#include <cstdlib>
#include "db.h"
#include "ServerManager.h"
#include "SuperCallback.h"
#include "LoginCallback.h"
#include <sstream>

using namespace std;
using namespace loki;

SuperServer::SuperServer(const std::string& script_file, io_service_pool& pool)
	:service(pool),
	codec_(std::bind(&SuperServer::do_msg, this, std::placeholders::_1, std::placeholders::_2)),
	script_file_(script_file)
{
}

void SuperServer::do_msg(connection_ptr conn, MessagePtr msg)
{
	get_logic_service().post(boost::bind(&protobuf_dispatcher::onProtobufMessage, &dispatcher_, conn, msg));
}

void SuperServer::on_connected(connection_ptr conn)
{
	context_ptr task(new ServerTask(conn));
	conn->set_context(task);
	get_logic_service().post(boost::bind(&SuperServer::add_connection, this, conn));
}

void SuperServer::add_connection(connection_ptr conn)
{
	connections_[conn->get_id()] = conn;
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
}

void SuperServer::remove_connection(connection_ptr conn)
{
	context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
	conn->set_context(context_ptr(nullptr));	//删除引用，避免循环引用,造成内存泄漏

	sub_conns_.erase(context->id());
	connections_.erase(conn->get_id());
	ServerManager::instance().erase(context->id());
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
}

void SuperServer::on_msg(connection_ptr conn)
{
	codec_.handle_message(conn);
}

void SuperServer::on_error(connection_ptr conn)
{
	get_logic_service().post(boost::bind(&SuperServer::remove_connection, this, conn));	
}

bool SuperServer::init()
{
	new ServerManager();
	setup_signal_callback();
	script_.reset(new script());
	if (!script_->dofile(script_file_))
	{
		return false;
	}

	lua_tinker::table common = script_->get<lua_tinker::table>("Common");

	port_ = (uint16_t)atoi(common.get<const char*>("super_port"));
	//as server
	server_.reset(new server(common.get<const char*>("super_ip"), common.get<const char*>("super_port"), pool_));
	server_->set_msg_handler( std::bind(&SuperServer::on_msg, this, std::placeholders::_1));
	server_->set_connected_handler( std::bind(&SuperServer::on_connected, this, std::placeholders::_1));
	server_->set_error_handler(std::bind(&SuperServer::on_error, this, std::placeholders::_1));
	server_->start_accept();

	//as client
	flclient_.reset(new connection(pool_.get_io_service()));
	flclient_->set_connected_handler(std::bind(&SuperServer::on_login_connected, this, std::placeholders::_1));	//TODO: change another handler
	flclient_->set_msg_handler(std::bind(&SuperServer::on_msg, this, std::placeholders::_1));
	//flclient_->set_error_handler(std::bind(&SuperServer::on_error, this, std::placeholders::_1));			//TODO: change another handler

	lua_tinker::table logintable = script_->get<lua_tinker::table>("SuperServer");
	flclient_->async_connect(logintable.get<const char*>("login_ip"), logintable.get<uint16_t>("login_port"));

	pool_.post_all(boost::bind(&SuperServer::init_thread_data, this));

	//{init db
	sql::Driver * driver = get_driver_instance();
	if(!driver){
		LOG(ERROR)<<("get driver instance error");
		return false;
	}
	s_dbConn.reset(driver->connect(common.get<const char*>("hostname"), common.get<const char*>("user"), common.get<const char*>("password")));
	if(!s_dbConn){
		LOG(ERROR)<<("connect mysql error");
		return false;
	}
	s_dbConn->setSchema(common.get<const char*>("database"));
	//}

	LoadServerList();

	registerCallback();
	return true;
}

void SuperServer::LoadServerList()
{
	serverlist_.ses_.clear();
	lua_tinker::table list = script_->get<lua_tinker::table>("ServerList");
	int size = lua_tinker::call<int>(script_->GetLuaState(), "GetTableSize", list);
	LOG(INFO)<< "ServerList size = "<<size;
	for (size_t i=0;i<size; ++i)
	{
		lua_tinker::table item = list.get_index<lua_tinker::table>(i+1);
		std::shared_ptr<Super::ServerEntry> se(new Super::ServerEntry);
		se->set_id(item.get<uint32_t>("id"));
		se->set_type(item.get<uint32_t>("_type"));
		se->set_name(item.get<const char*>("name"));
		se->set_ip(item.get<const char*>("ip"));
		se->set_port(item.get<uint32_t>("port"));
		se->set_extip(item.get<const char*>("extip"));
		se->set_extport(item.get<uint32_t>("extport"));
		se->set_nettype(item.get<uint32_t>("nettype"));

		serverlist_.ses_.push_back(se);
	}
	if(serverlist_.ses_.empty())
	{
		LOG(ERROR)<<("serverlist is empty");
		return ;
	}
	LOG(INFO)<<"serverlist size="<<serverlist_.ses_.size();

	serverSequence[SESSIONSERVER].push_back(RECORDSERVER);
	serverSequence[SCENESERVER].push_back(RECORDSERVER);
	serverSequence[SCENESERVER].push_back(SESSIONSERVER);

	serverSequence[GATEWAYSERVER].push_back(SESSIONSERVER);
	serverSequence[GATEWAYSERVER].push_back(RECORDSERVER);
	serverSequence[GATEWAYSERVER].push_back(SCENESERVER);
	/*
	   serverSequence[GATEWAYSERVER].push_back(BILLSERVER);
	*/

	//init dependency
	for (auto it = serverSequence.begin(); it != serverSequence.end(); ++it)
	{
		for (auto n = it->second.begin(); n != it->second.end(); ++n)
		{
			std::vector<int> a(serverlist_.getIDByType(*n));
			if (!a.empty())
			{
				std::copy(a.begin(), a.end(), back_inserter(dependency_[it->first]));
			}
		}
	}
}


void SuperServer::init_thread_data()
{
}

void SuperServer::registerCallback()
{
	using namespace Super;
	//from sub server
	dispatcher_.registerMsgCallback<Super::t_Startup_Request>(std::bind(onStartup_Request, std::placeholders::_1, std::placeholders::_2));

	//from Gateway
	dispatcher_.registerMsgCallback<Super::t_GYList_Gateway>(std::bind(onGYList_Gateway, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Super::t_NewSession_Gateway>(std::bind(on_NewSession_Gateway, std::placeholders::_1, std::placeholders::_2));

	//from login
	dispatcher_.registerMsgCallback<Login::t_LoginFL_OK>(std::bind(onRetZoneLogin, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Login::PlayerLogin>(std::bind(onPlayerLogin, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Login::t_NewSession_Session>(std::bind(on_NewSession_Session, std::placeholders::_1, std::placeholders::_2));
}

//查找一个可用id
bool SuperServer::checkZoneConn(connection_ptr conn, const uint32_t type)
{
	return true;
}

//{ 连接到 LoginServer 时的回调
void SuperServer::on_login_connected(connection_ptr conn)
{
	LOG(INFO)<<"login server connected, ip="<<conn->get_peer_ip()<<",port="<<conn->get_peer_port();
	get_logic_service().post(boost::bind(&SuperServer::loginFL, this, conn));
}

void SuperServer::loginFL(connection_ptr conn)
{
	Login::ReqZoneLogin login;
	login.set_openport(port_);
	codec_.send(conn, &login);
}
//} end login


std::vector<int> SuperServer::getDependencyID(const int type) const
{
	auto it = dependency_.find(type);
	if (it != dependency_.end())
		return it->second;
	else
		return std::vector<int>();
}

