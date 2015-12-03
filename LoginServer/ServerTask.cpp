#include "ServerTask.h"
#include "script.h"
#include "ThreadLocal.h"

ServerTask::ServerTask(connection_ptr conn)
	:context(conn)
{
	char key[32]= {'a', 'b', 'c'};
	aes_.setkey((const unsigned char*)key,256);
}

bool ServerTask::msg_parse(const void* ptr, const size_t len)
{
	unsigned char * rev = (unsigned char*)malloc(len + 1);
	//aes_.decrypt((const unsigned char*)ptr, len, (unsigned char*)rev);
	memcpy(rev, ptr, len);
	rev[len] = 0;

	//do msg
	lua_tinker::call<bool>(t_script_->get_state(), "MsgParse", (const char*)rev, len);
	free(rev);
}

ServerTask::~ServerTask()
{
}
