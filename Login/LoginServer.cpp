#include "LoginServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "db.h"
#include "UserCallback.h"
#include "SuperCallback.h"

using namespace loki;

LoginServer::LoginServer(io_service_pool& p): service(p)
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

	LoadAcl();
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
	using namespace Login;

	superDispatcher_.registerMsgCallback<Login::ReqZoneLogin>(std::bind(OnZoneLogin, std::placeholders::_1, std::placeholders::_2));

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
