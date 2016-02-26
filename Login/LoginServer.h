#pragma once

#include "TcpServer.h"
#include "singleton.h"
#include <string>
#include "script.h"
#include "Proto.h"
#include <vector>
#include "service.h"
#include <unordered_map>
#include "ConnEntity.h"

using namespace loki;

class LoginServer : public Singleton<LoginServer>, service
{
	public:
		std::map<int, std::vector<int> > serverSequence; //type->types
		LoginServer(loki::io_service_pool& pool);
		bool Init(const std::string& file);

		std::shared_ptr<script> script_;

		std::shared_ptr<TcpServer> server;
		ProtoDispatcher superDispatcher_;
		void HandleErrorSuperClient(TcpConnPtr conn, const boost::system::error_code& err);
		void RegisterCallback();

		uint16_t port_{0};
		std::vector<int> getDependencyID(const int type) const;
		//服务器之间的依赖
		std::map<int, std::vector<int> > dependency_;       //type->ids

		std::unordered_map<uint32_t, ConnEntity*> sub_conns_;

		//for user
		std::shared_ptr<TcpServer> userServer;
		ProtoDispatcher userDispatcher_;
		void HandleErrorUserClient(TcpConnPtr conn, const boost::system::error_code& err);
};
