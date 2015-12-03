#pragma once

#include "aes.h"
#include "connection.h"
#include "context.h"

using namespace loki;

class SessionTask : public loki::context
{
private:
	//connection_ptr conn_;
	aes_crypto aes_;
	
public:
	SessionTask(connection_ptr conn);
	~SessionTask();
};
