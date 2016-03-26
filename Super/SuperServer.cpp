#include "SuperServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "LoginCallback.h"

using namespace loki;

SuperServer::SuperServer(io_service_pool& p): service(p)
{
}

bool SuperServer::Init(const std::string& filename)
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

	lua_tinker::table common = script_->get<lua_tinker::table>("Common");
	port_ = (uint16_t)atoi(common.get<const char*>("super_port"));
	try {
		server.reset(new TcpServer(pool_, common.get<const char*>("super_ip"), common.get<const char*>("super_port")));
	}
	catch(...)
	{
		LOG(ERROR)<<"Create TcpServer Failed, normally it is that the port is in use";
		return false;
	}
	server->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &dispatcher_, std::placeholders::_1, std::placeholders::_2);
	server->start_accept();

	LoadServerList();

	lua_tinker::table superTable = script_->get<lua_tinker::table>("SuperServer");
	loginClient.reset(new TcpConnection(pool_.get_io_service()));
	loginClient->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &loginDispatcher_, std::placeholders::_1, std::placeholders::_2);
	loginClient->errorHandler = std::bind(&SuperServer::disconnectLogin, this, std::placeholders::_1, std::placeholders::_2);
	loginClient->connectedHandler = std::bind(&SuperServer::handleConnectLogin, this, std::placeholders::_1, std::placeholders::_2);
	loginClient->AsyncConnect(superTable.get<const char*>("login_ip"), superTable.get<uint16_t>("login_port"));
	return true;
}

void SuperServer::disconnectLogin(TcpConnPtr conn, const boost::system::error_code& err)
{
}

void SuperServer::handleConnectLogin(TcpConnPtr conn, const boost::system::error_code& err)
{
	if (!err)
	{
		LOG(INFO)<<"Connect to login server success";
		Login::ReqZoneLogin send;
		send.set_openport(port_);
		conn->SendMessage(&send);
	}
	else
	{
		LOG(INFO)<<"Connect to login server failed";
	}
}

void SuperServer::RegisterCallback()
{
	using namespace Super;
	dispatcher_.registerMsgCallback<Super::t_Startup_Request>(std::bind(onStartup_Request, std::placeholders::_1, std::placeholders::_2));

	loginDispatcher_.registerMsgCallback<Login::t_LoginFL_OK>(std::bind(OnRetZoneLogin, std::placeholders::_1, std::placeholders::_2));
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

std::vector<int> SuperServer::getDependencyID(const int type) const
{
	auto it = dependency_.find(type);
	if (it != dependency_.end())
		return it->second;
	else
		return std::vector<int>();
}
