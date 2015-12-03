#include "GatewayServer.h"
#include <iostream>
#include "logger.h"
#include <unistd.h>
#include <memory>
#include "util.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace loki;

int main(int argc, char** argv)
{
	Global::ParseCommand(argc, argv);
	bool daem = Global::Get<bool>("Daemon");

	if (daem)
	{
		int ret = daemon(1,1);
	}
	InitLog(argv[0]);

	io_service_pool pool(Global::Get<uint32_t>("ThreadNum"));
	std::unique_ptr<GatewayServer> ptr(new GatewayServer(pool, "script/gate.lua"));
	if (GatewayServer::getSingleton().init())
		pool.run();
	return 0;
}
