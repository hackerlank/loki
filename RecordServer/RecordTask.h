#pragma once

#include "aes.h"
#include "connection.h"
#include <queue>
#include "context.h"

using namespace loki;

class RecordTask : public loki::context
{
private:
	//connection_ptr conn_;
	aes_crypto aes_;
	
public:
	RecordTask(connection_ptr conn);
	~RecordTask();
};
