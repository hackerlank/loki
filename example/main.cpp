#include "GateServer.h"
#include <iostream>
#include "logger.h"

using namespace std;
using namespace loki;


int main(int argc, char** argv)
{
	InitLog(argv[0]);
	int n = 1;
	if (argc < 2)
	{
		cout<<"Usage: main threads"<<endl;
		return 0;
	}
	int threadnum = atoi(argv[1]);
	if (threadnum > 1)
	{
		n = threadnum;
	}

	io_service_pool pool(n);
	GateServer s("localhost", "11333", pool);
	pool.run();

	return 0;
}
