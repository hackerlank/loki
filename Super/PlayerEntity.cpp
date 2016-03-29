#include "PlayerEntity.h"

using namespace loki;

PlayerEntity::PlayerEntity(TcpConnPtr conn): ConnEntity(conn),
	searchFight(false)
{
}
