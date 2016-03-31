#pragma once

#include "aes.h"
#include "connection.h"
#include <queue>
#include "protobuf_codec.h"
#include "context.h"

using namespace loki;

class ServerTask : public loki::context
{
private:
	//connection_ptr conn_;
	aes_crypto aes_;
	
public:
	ServerTask(connection_ptr conn);
	~ServerTask();
};
