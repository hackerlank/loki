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

class SceneServer : public Singleton<SceneServer>, SubService
{
	public:
		SceneServer(loki::io_service_pool& pool, const std::string& filename);
		bool Init();

		//ProtoDispatcher dispatcher_;

		void RegisterCallback();

		bool PostInit();

		ProtoDispatcher recordDispatcher;
		TcpConnPtr recordClient;
		void disconnectRecord(TcpConnPtr conn, const boost::system::error_code& err);

		ProtoDispatcher sessionDispatcher;
		TcpConnPtr sessionClient;
		void disconnectSession(TcpConnPtr conn, const boost::system::error_code& err);
};
