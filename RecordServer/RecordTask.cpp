#include "RecordTask.h"
#include "script.h"

RecordTask::RecordTask(connection_ptr conn)
	:context(conn)
{
	char key[32]= {'a', 'b', 'c'};
	aes_.setkey((const unsigned char*)key,256);
}

RecordTask::~RecordTask()
{
}