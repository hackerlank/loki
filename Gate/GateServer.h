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

class GateServer : public Singleton<GateServer>, SubService
{
	public:
		GateServer(loki::io_service_pool& pool, const std::string& filename);
		bool Init();

		void RegisterCallback();

		bool PostInit();

		ProtoDispatcher recordDispatcher;
		TcpConnPtr recordClient;
		void disconnectRecord(TcpConnPtr conn, const boost::system::error_code& err);

		ProtoDispatcher sessionDispatcher;
		TcpConnPtr sessionClient;
		void disconnectSession(TcpConnPtr conn, const boost::system::error_code& err);

		ProtoDispatcher sceneDispatcher;
		std::map<uint32_t, TcpConnPtr> sceneClients;
		void disconnectScene(TcpConnPtr conn, const boost::system::error_code& err);
		void handleConnectScene(TcpConnPtr, const boost::system::error_code& error);
};
