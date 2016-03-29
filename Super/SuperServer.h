#pragma once

#include "TcpServer.h"
#include "singleton.h"
#include <string>
#include "script.h"
#include "Proto.h"
#include "ServerList.h"
#include <vector>
#include "Service.h"
#include <unordered_map>
#include "ConnEntity.h"
#include "TcpConn.h"

using namespace loki;

class SuperServer : public Singleton<SuperServer>, Service
{
	public:
		std::map<int, std::vector<int> > serverSequence; //type->types
		SuperServer(loki::io_service_pool& pool);
		std::shared_ptr<TcpServer> server;
		bool Init(const std::string& file);

		ProtoDispatcher dispatcher_;
		void RegisterCallback();

		uint16_t port_{0};
		std::string ip_;
		ServerList serverlist_;
		void LoadServerList();
		std::vector<int> getDependencyID(const int type) const;
		//服务器之间的依赖
		std::map<int, std::vector<int> > dependency_;       //type->ids

		std::unordered_map<uint32_t, ConnEntity*> sub_conns_;
		ServerList& get_serverlist() { return serverlist_; }

		TcpConnPtr loginClient;
		ProtoDispatcher loginDispatcher_;

		void disconnectLogin(TcpConnPtr conn, const boost::system::error_code& err, const std::string& hint);
		void handleConnectLogin(TcpConnPtr conn);

		uint32_t zone_;
		uint32_t game_;
		std::string name_;

		void OnAccept(TcpConnPtr conn);
		void OnError(TcpConnPtr conn, const boost::system::error_code& err, const std::string& hint);
		void Run(long delta);
};
