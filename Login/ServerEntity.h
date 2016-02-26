#pragma once
#include "context.h"

class ServerEntity : public ConnEntity
{
	public:
		ServerEntity(TcpConnPtr conn): ConnEntity(conn)
	{
	}
};
