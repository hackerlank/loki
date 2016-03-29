#include "LoginServer.h"
#include "LoginTask.h"
#include "ServerTask.h"
#include "ThreadLocal.h"
#include "LoginCallback.h"
#include "db.h"
#include "mutex.h"
#include "ServerManager.h"
#include "SuperCallback.h"
#include <boost/date_time/posix_time/posix_time.hpp>

thread_local script* t_script_ = nullptr;

using namespace loki;

LoginServer::LoginServer(const std::string& script_file, io_service_pool& pool)
	:Service(pool),
	codec_(std::bind(&LoginServer::do_msg, this, std::placeholders::_1, std::placeholders::_2)),
	script_file_(script_file)
{
}

void LoginServer::do_msg(connection_ptr conn, MessagePtr msg)
{
	get_logic_service().post(boost::bind(&protobuf_dispatcher::onProtobufMessage, &dispatcher_, conn, msg));
}

void LoginServer::on_server_connected(connection_ptr conn)
{
	ServerTaskPtr task(new ServerTask(conn));
	conn->set_context(task);
}

void LoginServer::add_user_connection(connection_ptr conn)
{
	user_conns_[conn->get_id()] = conn;
	LOG(INFO)<<__func__<<",conn size"<<user_conns_.size()<<", connid="<<conn->get_id();
}

void LoginServer::remove_user_connection(connection_ptr conn)
{
	auto it = user_conns_.find(conn->get_id());
	if (it != user_conns_.end())
	{
		user_conns_.erase(conn->get_id());
		LOG(INFO)<<__func__<<",conn size"<<user_conns_.size()<<", connid="<<conn->get_id();
	}
}

void LoginServer::on_msg(connection_ptr conn)
{
	codec_.handle_message(conn);
	/*
	loki::buffer* buffer = conn->get_buffer();
	std::string rev((const char*)buffer->data_ptr(), buffer->data_size());
	conn->send(rev);
	buffer->consume(buffer->data_size());
	if (rev == "quit")
	{
		conn->close();
	}
	*/
}

void LoginServer::on_error(connection_ptr conn)
{
}

void LoginServer::on_user_connected(connection_ptr conn)
{
	LoginTaskPtr task(new LoginTask(conn));
	conn->set_context(task);
}

//port : 10000
void LoginServer::on_user_msg(connection_ptr conn)
{
	codec_.handle_message(conn);
}

void LoginServer::on_user_error(connection_ptr conn)
{
	get_logic_service().post(boost::bind(&LoginServer::remove_user_connection, this, conn));	
}

bool LoginServer::init()
{
	new ServerManager();
	new GYListManager();
	setup_signal_callback();
	script_.reset(new script());
	if (!script_->dofile(script_file_))
	{
		return false;
	}

	lua_tinker::table logintable = script_->get<lua_tinker::table>("LoginServer");

	//初始化区连接的服务器
	//10001
	zone_server_.reset(new server(logintable.get<const char*>("server_ip"), logintable.get<const char*>("server_port"), pool_));
	zone_server_->set_msg_handler( std::bind(&LoginServer::on_msg, this, std::placeholders::_1));
	zone_server_->set_connected_handler(std::bind(&LoginServer::on_server_connected, this, std::placeholders::_1));
	zone_server_->set_error_handler(std::bind(&LoginServer::on_error, this, std::placeholders::_1));
	zone_server_->start_accept();

	//10000
	login_server_.reset(new server(logintable.get<const char*>("login_ip"), logintable.get<const char*>("login_port"), pool_));
	login_server_->set_msg_handler( std::bind(&LoginServer::on_user_msg, this, std::placeholders::_1));
	login_server_->set_connected_handler(std::bind(&LoginServer::on_user_connected, this, std::placeholders::_1));
	login_server_->set_error_handler(std::bind(&LoginServer::on_user_error, this, std::placeholders::_1));
	login_server_->start_accept();

	pool_.post_all(boost::bind(&LoginServer::init_thread_data, this));

	lua_tinker::table common = script_->get<lua_tinker::table>("Common");
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
	loadAcl();

	registerCallback();
	return true;
}

void LoginServer::init_thread_data()
{
	if (t_script_)
	{
		return ;
	}
	t_script_ = new script();
	t_script_->dofile(script_file_);
	LOG(INFO)<<("run in every thread");
}

void LoginServer::registerCallback()
{
	using namespace Login;
	//user msg
	dispatcher_.registerMsgCallback<Login::stUserVerifyVerCmd>(std::bind(verifyUserConn, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Login::stUserRequestLoginCmd>(std::bind(userRequestLogin, std::placeholders::_1, std::placeholders::_2));

	//zone server msg
	dispatcher_.registerMsgCallback<Login::ReqZoneLogin>(std::bind(onZoneLogin, std::placeholders::_1, std::placeholders::_2));

	dispatcher_.registerMsgCallback<Login::t_GYList_FL>(std::bind(onGYList_FL, std::placeholders::_1, std::placeholders::_2));	
	dispatcher_.registerMsgCallback<Login::t_NewSession_Session>(std::bind(onNewSession_Session, std::placeholders::_1, std::placeholders::_2));
}

bool LoginServer::loadAcl()
{
	loki::scoped_lock lock(mutex_);
	aclmap.clear();

	std::shared_ptr< sql::Statement > stmt(s_dbConn->createStatement());
	std::shared_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT * FROM `zonelist`"));
	while(res->next())
	{
		std::shared_ptr<ACLZone> se(new ACLZone);

		se->gamezone.game = res->getUInt("game");
		se->gamezone.zone = res->getUInt("zone");
		se->name = res->getString("name");
		se->ip = res->getString("ip");
		se->port = res->getUInt("port");
		se->desc = res->getString("description");

		if (res->getUInt("isuse"))
		{
			aclmap[se->gamezone] = se;
		}

	}
	LOG(INFO)<<"load acl size="<<aclmap.size();
	return true;
}

std::shared_ptr<ACLZone> LoginServer::getAcl(const std::string& ip, const uint16_t port)
{
	loki::scoped_lock lock(mutex_);
	std::shared_ptr<ACLZone> ret;
	for (auto it = aclmap.begin(); it != aclmap.end(); ++it)
	{
		if (ip == it->second->ip && port == it->second->port)
		{
			return it->second;
		}
	}
	return ret;
}

bool LoginServer::broadcastByID(const uint64_t conn_id, const MessagePtr& msg)
{
	return broadcastByID(conn_id, msg.get());
}

bool LoginServer::broadcastByID(const uint64_t conn_id, const google::protobuf::Message* msg)
{
	auto conn = user_conns_.find(conn_id);
	if (conn == user_conns_.end()) return false;
	protobuf_codec::send(conn->second, msg);
	return true;
}
