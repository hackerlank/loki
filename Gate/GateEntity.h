#pragma once
#include "ConnEntity.h"
#include "ServerList.h"

class GateEntity : public loki::ConnEntity
{
	public:
		GateEntity(TcpConnPtr conn): loki::ConnEntity(conn)
	{
	}
		ServerEntryPtr entryPtr;
};
