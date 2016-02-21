#pragma once

#include "TcpServer.h"
#include "singleton.h"
#include <string>
#include "script.h"
#include "Proto.h"
#include "ServerList.h"
#include <vector>
#include "SubService.h"

using namespace loki;

class SessionServer : public Singleton<SessionServer>, SubService
{
	public:
		SessionServer(loki::io_service_pool& pool, const std::string& filename);
		//std::shared_ptr<TcpServer> server;
		bool Init();

		//std::shared_ptr<script> script_;

		//ProtoDispatcher dispatcher_;

		//TcpConnPtr superClient;
		//void HandleConnectedSuper(TcpConnPtr, const boost::system::error_code& error);
		void RegisterCallback();

		bool PostInit();

		ProtoDispatcher recordDispatcher;
		TcpConnPtr recordClient;

		void disconnectRecord(TcpConnPtr conn, const boost::system::error_code& err);
};
