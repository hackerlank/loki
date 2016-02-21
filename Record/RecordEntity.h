#pragma once
#include "ConnEntity.h"

class RecordEntity : public loki::ConnEntity
{
	public:
		RecordEntity(TcpConnPtr conn): loki::ConnEntity(conn)
	{
	}
};
