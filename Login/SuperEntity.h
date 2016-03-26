#pragma once
#include "context.h"
#include "ACLZone.h"
#include <memory>

class SuperEntity : public ConnEntity
{
	public:
		SuperEntity(TcpConnPtr conn): ConnEntity(conn)
	{
	}
		std::shared_ptr<ACLZone> zoneInfo;
};
