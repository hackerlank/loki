#pragma once

#include "TcpServer.h"
#include "singleton.h"
#include <string>
#include "script.h"
#include "Proto.h"
#include <vector>
#include "Service.h"
#include <unordered_map>
#include "ConnEntity.h"
#include "ConnContainer.h"
#include "gamezone.h"
#include "ACLZone.h"

using namespace loki;

class LoginServer : public Singleton<LoginServer>, Service
{
	public:
		std::map<int, std::vector<int> > serverSequence; //type->types
		LoginServer(loki::io_service_pool& pool);
		bool Init(const std::string& file);

		ConnContainer serverConns;
		ConnContainer userConns;

		std::shared_ptr<TcpServer> server;
		ProtoDispatcher superDispatcher_;
		void RegisterCallback();

		uint16_t port_{0};
		std::vector<int> getDependencyID(const int type) const;
		//服务器之间的依赖
		std::map<int, std::vector<int> > dependency_;       //type->ids

		std::unordered_map<uint32_t, ConnEntity*> sub_conns_;

		//for user
		std::shared_ptr<TcpServer> userServer;
		ProtoDispatcher userDispatcher_;
		void OnErrorClient(TcpConnPtr conn, const boost::system::error_code& err, const std::string& hint);

		void OnErrorServer(TcpConnPtr conn, const boost::system::error_code& err, const std::string& hint);

		void OnAcceptClient(TcpConnPtr Conn);
		void OnAcceptServer(TcpConnPtr Conn);


		bool LoadAcl();

		std::unordered_map<GameZone_t, std::shared_ptr<ACLZone> > aclmap;
		std::shared_ptr<ACLZone> getAcl(const std::string& ip, const uint16_t port);

		std::mutex mutex_;
};
