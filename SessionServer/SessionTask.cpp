#include "SessionTask.h"
#include "script.h"

SessionTask::SessionTask(connection_ptr conn)
	:context(conn)
{
	char key[32]= {'a', 'b', 'c'};
	aes_.setkey((const unsigned char*)key,256);
}

SessionTask::~SessionTask()
{
}
