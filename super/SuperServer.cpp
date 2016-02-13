#include "SuperServer.h"
#include "Callback.h"
#include "ServerType.h"

using namespace loki;

SuperServer::SuperServer(io_service_pool& p): pool(p)
{
}

bool SuperServer::Init(const std::string& filename)
{
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
	server.reset(new TcpServer(pool, common.get<const char*>("super_ip"), common.get<const char*>("super_port")));
	server->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &dispatcher_, std::placeholders::_1, std::placeholders::_2);
	server->start_accept();

	LoadServerList();
}

void SuperServer::RegisterCallback()
{
	dispatcher_.registerMsgCallback<Super::t_Startup_Request>(std::bind(onStartup_Request, std::placeholders::_1, std::placeholders::_2));

	dispatcher_.registerMsgCallback<Login::stUserVerifyVerCmd>(std::bind(onUserVerifyVerCmd, std::placeholders::_1, std::placeholders::_2));
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
