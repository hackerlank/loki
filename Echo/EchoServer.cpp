#include <iostream>
#include <unistd.h>
#include "logger.h"
#include "io_service_pool.h"
#include "TcpServer.h"

using namespace std;
using namespace loki;

class Service
{

public:
	void OnMessage(TcpConnPtr conn, const char* msg, const uint32_t size)
	{
		//LOG(INFO)<<std::string(msg, size);
		conn->SendMessage(msg, size);
	}
	void OnConnected(TcpConnPtr conn)
	{
		LOG(INFO)<<__func__;
	}
	void OnError(TcpConnPtr conn, const boost::system::error_code& err, const string& hint)
	{
		LOG(INFO)<<__func__;
	}
};


int main(int argc, char** argv)
{
	/*
	Global::ParseCommand(argc, argv);
	if (Global::Get<bool>("Daemon"))
	{
		int ret = daemon(1,1);
	}
	*/
	LoggerHelper::InitLog(argv[0]);

	io_service_pool pool(2);
	Service* service(new Service());
	TcpServer* server(new TcpServer(pool, "localhost", "33300"));
	server->msgHandler = std::bind(&Service::OnMessage, service, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	server->connectedHandler = std::bind(&Service::OnConnected, service, std::placeholders::_1);
	server->errorHandler = std::bind(&Service::OnError, service, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	server->start_accept();
	pool.run();
	return 0;
}
