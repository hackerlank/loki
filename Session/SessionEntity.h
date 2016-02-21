#pragma once
#include "ConnEntity.h"

class SessionEntity : public loki::ConnEntity
{
	public:
		SessionEntity(TcpConnPtr conn): loki::ConnEntity(conn)
	{
	}
};
