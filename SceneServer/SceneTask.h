#pragma once

#include "connection.h"
#include "context.h"

using namespace loki;

class SceneTask : public loki::context
{
private:
	//connection_ptr conn_;
	
public:
	SceneTask(connection_ptr conn);
	~SceneTask();
};
