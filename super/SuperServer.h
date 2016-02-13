#pragma once

#include "TcpServer.h"
#include "singleton.h"
#include <string>
#include "script.h"
#include "Proto.h"
#include "ServerList.h"
#include <vector>

using namespace loki;

class SuperServer : public Singleton<SuperServer>
{
	private:
		loki::io_service_pool& pool;
	public:
		std::map<int, std::vector<int> > serverSequence; //type->types
		SuperServer(loki::io_service_pool& pool);
		std::shared_ptr<TcpServer> server;
		bool Init(const std::string& file);

		std::shared_ptr<script> script_;

		ProtoDispatcher dispatcher_;
		void RegisterCallback();

		uint16_t port_{0};
		ServerList serverlist_;
		void LoadServerList();
		std::vector<int> getDependencyID(const int type) const;
		//服务器之间的依赖
		std::map<int, std::vector<int> > dependency_;       //type->ids
};
