#pragma once

#include "TcpServer.h"
#include "singleton.h"
#include <string>
#include "script.h"
#include "Proto.h"
#include "ServerList.h"
#include <vector>
#include "service.h"
#include <unordered_map>
#include "ConnEntity.h"
#include "TcpConn.h"

using namespace loki;

class SuperServer : public Singleton<SuperServer>, service
{
	public:
		std::map<int, std::vector<int> > serverSequence; //type->types
		SuperServer(loki::io_service_pool& pool);
		std::shared_ptr<TcpServer> server;
		bool Init(const std::string& file);

		ProtoDispatcher dispatcher_;
		void RegisterCallback();

		uint16_t port_{0};
		ServerList serverlist_;
		void LoadServerList();
		std::vector<int> getDependencyID(const int type) const;
		//服务器之间的依赖
		std::map<int, std::vector<int> > dependency_;       //type->ids

		std::unordered_map<uint32_t, ConnEntity*> sub_conns_;
		ServerList& get_serverlist() { return serverlist_; }

		TcpConnPtr loginClient;
		ProtoDispatcher loginDispatcher_;

		void disconnectLogin(TcpConnPtr conn, const boost::system::error_code& err);
		void handleConnectLogin(TcpConnPtr conn, const boost::system::error_code& err);

		uint32_t zone_;
		uint32_t game_;
		std::string name_;

};
