#pragma once
#include "ConnEntity.h"

class SceneEntity : public loki::ConnEntity
{
	public:
		SceneEntity(TcpConnPtr conn): loki::ConnEntity(conn)
	{
	}
};
