#include "LoginTask.h"
#include "script.h"
#include "ThreadLocal.h"
#include "encryptor_aes.h"

LoginTask::LoginTask(connection_ptr conn)
	:context(conn)
{
}

bool LoginTask::msg_parse(const void* ptr, const size_t len)
{
	unsigned char * rev = (unsigned char*)malloc(len + 1);
	//aes_.decrypt((const unsigned char*)ptr, len, (unsigned char*)rev);
	memcpy(rev, ptr, len);
	rev[len] = 0;

	//do msg
	lua_tinker::call<bool>(t_script_->get_state(), "MsgParse", (const char*)rev, len);
	free(rev);
}

LoginTask::~LoginTask()
{
}
