#include "LoginServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "db.h"
#include "UserCallback.h"
#include "SuperCallback.h"
#include "GYListManager.h"

using namespace loki;

LoginServer::LoginServer(io_service_pool& p): Service(p)
{
	new GYListManager();
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
		server->connectedHandler = std::bind(&LoginServer::OnAcceptServer, this, std::placeholders::_1);
		server->errorHandler = std::bind(&LoginServer::OnErrorServer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		server->start_accept();

		userServer.reset(new TcpServer(pool_, loginServer.get<const char*>("login_ip"), loginServer.get<const char*>("login_port")));
		userServer->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &userDispatcher_, std::placeholders::_1, std::placeholders::_2);
		userServer->errorHandler = std::bind(&LoginServer::OnErrorClient, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		userServer->connectedHandler = std::bind(&LoginServer::OnAcceptClient, this, std::placeholders::_1);
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

	LoadAcl();
	StartTimer();
	return true;
}

void LoginServer::OnAcceptServer(TcpConnPtr conn)
{
	LOG(INFO)<<__func__;
}

void LoginServer::OnAcceptClient(TcpConnPtr conn)
{
	LOG(INFO)<<__func__;
}

void LoginServer::OnErrorServer(TcpConnPtr conn, const boost::system::error_code& err, const std::string& hint)
{
	LOG(INFO)<<__func__;
	//remove connection from container
}

void LoginServer::OnErrorClient(TcpConnPtr conn, const boost::system::error_code& err, const std::string& hint)
{
	LOG(INFO)<<__func__;
	//remove connection from container
}

void LoginServer::RegisterCallback()
{
	using namespace Login;

	//for SuperServer
	superDispatcher_.registerMsgCallback<Login::ReqZoneLogin>(std::bind(OnZoneLogin, std::placeholders::_1, std::placeholders::_2));
	superDispatcher_.registerMsgCallback<Login::t_GYList_FL>(std::bind(OnGYList_FL, std::placeholders::_1, std::placeholders::_2));
	superDispatcher_.registerMsgCallback<Login::t_NewSession_Session>(std::bind(onNewSession_Session, std::placeholders::_1, std::placeholders::_2));

	//for client
	userDispatcher_.registerMsgCallback<Login::stUserVerifyVerCmd>(std::bind(OnUserVerifyVerCmd, std::placeholders::_1, std::placeholders::_2));
	userDispatcher_.registerMsgCallback<Login::stUserRequestLoginCmd>(std::bind(OnUserRequestLogin, std::placeholders::_1, std::placeholders::_2));
}

std::vector<int> LoginServer::getDependencyID(const int type) const
{
	auto it = dependency_.find(type);
	if (it != dependency_.end())
		return it->second;
	else
		return std::vector<int>();
}

bool LoginServer::LoadAcl()
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
