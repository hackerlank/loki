#include "ServerTask.h"
#include "script.h"
#include "compressor_zlib.h"

ServerTask::ServerTask(connection_ptr conn)
	:context(conn)
{
	char key[32]= {'a', 'b', 'c'};
	aes_.setkey((const unsigned char*)key,256);
	conn->set_compressor(std::shared_ptr<loki::compressor>(new loki::compressor_zlib()));
}

ServerTask::~ServerTask()
{
}

