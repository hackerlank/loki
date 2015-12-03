#pragma once

#include "connection.h"
#include "context.h"

using namespace loki;

class LoginTask;
typedef std::shared_ptr<LoginTask> LoginTaskPtr;

class LoginTask : public loki::context
{
private:
	uint32_t version_{0};
public:
	void set_version(const uint32_t version) { version_ = version; }
	LoginTask(connection_ptr conn);
	bool msg_parse(const void* ptr, const size_t len);
	~LoginTask();
};
