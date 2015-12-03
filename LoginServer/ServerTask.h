#pragma once

#include "aes.h"
#include "connection.h"
#include "context.h"
#include "gamezone.h"
#include "ACLZone.h"

using namespace loki;

class ServerTask;
typedef std::shared_ptr<ServerTask> ServerTaskPtr;

class ServerTask : public loki::context
{
private:
	aes_crypto aes_;

	ACLZone zoneinfo_;
public:
	ServerTask(connection_ptr conn);
	bool msg_parse(const void* ptr, const size_t len);
	~ServerTask();

	void set_zoneinfo(const std::shared_ptr<ACLZone>& p) { zoneinfo_ = *p; }

	GameZone_t gamezone() const { return zoneinfo_.gamezone; }
	std::string name() const { return zoneinfo_.name; }
};
