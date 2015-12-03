#include "RecordServer.h"
#include <iostream>
#include "logger.h"
#include <unistd.h>

using namespace std;
using namespace loki;

int main(int argc, char** argv)
{
	InitLog(argv[0]);
	int thread = 1;
	bool daem = false;

	int ch = 0;
	while ((ch = getopt(argc, argv, "t:d"))!= -1)
	{
		switch (ch)
		{
			case 't':
				{
					thread = atoi(optarg);
				}
				break;
			case 'd':
				{
					daem = true;
				}
				break;
		}
	}
	if (daem)
	{
		int ret = daemon(1,1);
	}

	io_service_pool pool(thread);
	std::unique_ptr<RecordServer> p(new RecordServer(pool, "script/login.lua"));
	if (RecordServer::instance().init())
		pool.run();
	return 0;
}
