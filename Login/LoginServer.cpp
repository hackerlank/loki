#include "LoginServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "db.h"

using namespace loki;

LoginServer::LoginServer(io_service_pool& p): service(p)
{
}

bool LoginServer::Init(const std::string& filename)
{
	setup_signal_callback();

	if (filename.empty())
	{
		LOG(ERROR)<<"Lack of scriptfile";
		return false;
	}
	script_.reset(new script());       
	if (!script_->dofile(filename))
	{                                  
		return false;                  
	}                                  
	RegisterCallback();

	lua_tinker::table loginServer = script_->get<lua_tinker::table>("LoginServer");
	port_ = (uint16_t)atoi(loginServer.get<const char*>("server_port"));
	try {
		//for SuperServer
		server.reset(new TcpServer(pool_, loginServer.get<const char*>("server_ip"), loginServer.get<const char*>("server_port")));
		server->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &superDispatcher_, std::placeholders::_1, std::placeholders::_2);
		server->errorHandler = std::bind(&LoginServer::HandleErrorSuperClient, this, std::placeholders::_1, std::placeholders::_2);
		server->start_accept();

		userServer.reset(new TcpServer(pool_, loginServer.get<const char*>("login_ip"), loginServer.get<const char*>("login_port")));
		userServer->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &userDispatcher_, std::placeholders::_1, std::placeholders::_2);
		userServer->errorHandler = std::bind(&LoginServer::HandleErrorUserClient, this, std::placeholders::_1, std::placeholders::_2);
		userServer->start_accept();
	}
	catch(...)
	{
		LOG(ERROR)<<"Create TcpServer Failed, normally it is that the port is in use";
		return false;
	}
	sql::Driver * driver = get_driver_instance();
	if(!driver){
		LOG(ERROR)<<("get driver instance error");
		return false;
	}
	lua_tinker::table common = script_->get<lua_tinker::table>("Common");
	s_dbConn.reset(driver->connect(common.get<const char*>("hostname"), common.get<const char*>("user"), common.get<const char*>("password")));
	if(!s_dbConn){
		LOG(ERROR)<<("connect mysql error");
		return false;
	}
	s_dbConn->setSchema(common.get<const char*>("database"));
	LOG(INFO)<<"Connect Database Success";

	return true;
}

void LoginServer::HandleErrorSuperClient(TcpConnPtr conn, const boost::system::error_code& err)
{
	//remove connection from container
}

void LoginServer::HandleErrorUserClient(TcpConnPtr conn, const boost::system::error_code& err)
{
	//remove connection from container
}

void LoginServer::RegisterCallback()
{
	//dispatcher_.registerMsgCallback<Login::t_Startup_Request>(std::bind(onStartup_Request, std::placeholders::_1, std::placeholders::_2));

	//dispatcher_.registerMsgCallback<Login::stUserVerifyVerCmd>(std::bind(onUserVerifyVerCmd, std::placeholders::_1, std::placeholders::_2));
}

std::vector<int> LoginServer::getDependencyID(const int type) const
{
	auto it = dependency_.find(type);
	if (it != dependency_.end())
		return it->second;
	else
		return std::vector<int>();
}
