#include "GatewayServer.h"
#include "GatewayTask.h"
#include <cstdlib>
#include "GatewayCallback.h"
#include "ServerCallback.h"
#include "LoginSessionManager.h"
#include "GatewayTaskManager.h"
#include "SceneClient.h"
#include "SceneClientManager.h"

using namespace std;
using namespace loki;

GatewayServer::GatewayServer(io_service_pool& pool, const std::string& script_file)
	:sub_service(pool, GATEWAYSERVER, script_file),
	one_second_timer_(pool.get_logic_service())
{
}

void GatewayServer::on_connected(connection_ptr conn)
{
	context_ptr task(new GatewayTask(conn));
	conn->set_context(task);
	//get_logic_service().post(boost::bind(&GatewayServer::add_connection, this, conn));
}

void GatewayServer::add_connection(connection_ptr conn)
{
	connections_[conn->get_id()] = conn;
	LOG(INFO)<<__func__<<",conn size="<<connections_.size()<<", connid="<<conn->get_id();
}

void GatewayServer::remove_connection(connection_ptr conn)
{
	//context_ptr context(boost::any_cast<context_ptr>(conn->release_context()));
	//conn->set_context(context_ptr(nullptr));

	GatewayTaskPtr task = loki::conn_cast<GatewayTask>(conn);
	GatewayTaskManager::instance().removeCountryUser(task);
	GatewayTaskManager::instance().uniqueRemove(task);
	//connections_.erase(conn->get_id());
	LOG(INFO)<<__func__<<",conn size="<<get_users_num()<<", conn="<<conn->get_peer_info();
}

void GatewayServer::on_error(connection_ptr conn)
{
	get_logic_service().post(boost::bind(&GatewayServer::remove_connection, this, conn));	
}

void GatewayServer::init_thread_data()
{
}

void GatewayServer::registerCallback()
{
	using namespace Gate;

	//from user
	dispatcher_.registerMsgCallback<Gate::stUserVerifyVerCmd>(std::bind(verifyVersion, std::placeholders::_1, std::placeholders::_2));	//1 版本验证
	dispatcher_.registerMsgCallback<Gate::stPasswdLogonUserCmd>(std::bind(verifyAccid, std::placeholders::_1, std::placeholders::_2));	//1 版本帐号
	dispatcher_.registerMsgCallback<Gate::stCheckNameSelectUserCmd>(std::bind(checkUserName, std::placeholders::_1, std::placeholders::_2));	//1 版本帐号
	dispatcher_.registerMsgCallback<Gate::stCreateSelectUserCmd>(std::bind(onCreateCharacter, std::placeholders::_1, std::placeholders::_2));	//1 版本帐号
	dispatcher_.registerMsgCallback<Gate::stLoginSelectUserCmd>(std::bind(onLoginSelectUser, std::placeholders::_1, std::placeholders::_2));	//1 版本帐号

	//from super
	dispatcher_.registerMsgCallback<Super::t_RQGYList_Gateway>(std::bind(onRQGYList_Gateway, std::placeholders::_1, std::placeholders::_2));	//login server request gate info
	dispatcher_.registerMsgCallback<Super::t_NewSession_Gateway>(std::bind(onNewSession_Gateway, std::placeholders::_1, std::placeholders::_2));

	//from record
	dispatcher_.registerMsgCallback<Record::t_Ret_SelectInfo_GateRecord>(std::bind(onRetSelectUserInfo, std::placeholders::_1, std::placeholders::_2));	//返回角色基本信息
	dispatcher_.registerMsgCallback<Record::t_CheckName_Return_GateRecord>(std::bind(onRetCheckName, std::placeholders::_1, std::placeholders::_2));	//返回角色基本信息
	dispatcher_.registerMsgCallback<Record::t_CreateChar_Return_GateRecord>(std::bind(onRetCreateChar, std::placeholders::_1, std::placeholders::_2));
}

bool GatewayServer::start_server()
{
	new LoginSessionManager();
	new GatewayTaskManager();
	new SceneClientManager();

	notify_login_server();

	//as client
	recordclient_.reset(new connection(pool_.get_io_service(),"recordclient"));
	recordclient_->set_msg_handler(std::bind(&GatewayServer::on_msg, this, std::placeholders::_1));
	recordclient_->set_error_handler(std::bind(&GatewayServer::handle_stop, this));			//TODO: change another handler
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
			
	//as client
	sessionclient_.reset(new connection(pool_.get_io_service(), "sessionclient"));
	sessionclient_->set_msg_handler(std::bind(&GatewayServer::on_msg, this, std::placeholders::_1));
	sessionclient_->set_error_handler(std::bind(&GatewayServer::handle_stop, this));			//TODO: change another handler
	ServerEntryPtr sessionEntry = serverlist_.getEntryByType(SESSIONSERVER);
	if (!sessionEntry)
	{
		LOG(INFO)<<("can't find session entry");
		return false;
	}
	if (!sessionclient_->connect(sessionEntry->ip(), sessionEntry->port()))
	{
		LOG(INFO)<<("connect session error");
		return false;
	}
	else
	{
		Session::t_LoginSession send;
		send.set_id(myentry_->id());
		send.set_type(myentry_->type());
		codec_.send(sessionclient_, &send);
	}

	//as client
	std::vector<ServerEntryPtr> scenesentry = serverlist_.getEntrysByType(SCENESERVER);
	if (scenesentry.empty())
	{
		LOG(INFO)<<("can't find scene entry");
		return false;
	}
	else
	{
		LOG(INFO)<<"scene entry num="<<scenesentry.size();
	}

	SceneClientManager::instance().init(scenesentry);

	started_ = true;

	one_second_timer_.expires_from_now(boost::posix_time::seconds(1));
	one_second_timer_.async_wait(boost::bind(&GatewayServer::one_second_callback, this, _1));

	return true;
}

size_t GatewayServer::get_users_num() const
{
	return GatewayTaskManager::instance().size();
}

void GatewayServer::notify_login_server()
{
	Super::t_GYList_Gateway send;
	auto gateinfo = send.mutable_gateinfo();
	gateinfo->set_serverid(GatewayServer::instance().get_id());
	gateinfo->set_ip(GatewayServer::instance().get_ip());
	gateinfo->set_port(GatewayServer::instance().get_port());
	gateinfo->set_version(GatewayServer::instance().version());
	gateinfo->set_num_online(static_cast<uint32_t>(GatewayServer::instance().get_users_num()));
	protobuf_codec::send(superclient_, &send);
	LOG(INFO)<<"gateway users size ="<<gateinfo->num_online();
}

void GatewayServer::one_second_callback(const boost::system::error_code& e)
{
	if (e != boost::asio::error::operation_aborted)
	{
		ptime currentTime(microsec_clock::local_time());
		LoginSessionManager::instance().update(currentTime);

		one_second_timer_.expires_from_now(boost::posix_time::seconds(1));
		one_second_timer_.async_wait(boost::bind(&GatewayServer::one_second_callback, this, _1));
	}
}

void GatewayServer::handle_stop()
{
	/*
	for (auto a: sceneclients_)
	{
		//a.second->set_context(context_ptr(nullptr));
		a.second->set_reuse(false);
		a.second->close();
	}
	sceneclients_.clear();
	*/
	Service::handle_stop();

	//delete SceneClientManager::instance_ptr();
	//delete LoginSessionManager::instance_ptr();
	//delete GatewayTaskManager::instance_ptr();
}
