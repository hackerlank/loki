#pragma once
#include "ConnEntity.h"
#include "TcpConn.h"

class PlayerEntity : public loki::ConnEntity
{
	public:
		PlayerEntity(loki::TcpConnPtr conn);
		void SetSearchFight(bool fight) { searchFight = fight; }
		bool IsSearchFight() const { return searchFight; }
		uint32_t Accid() const { return accid; }
		void SetAccid(uint32_t a) { accid = a; }
	private:
		bool searchFight;
		uint32_t accid;
};
