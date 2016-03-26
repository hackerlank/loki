#pragma once
#include "context.h"

class UserEntity : public ConnEntity
{
	public:
		UserEntity(TcpConnPtr conn): ConnEntity(conn)
	{
	}
};
