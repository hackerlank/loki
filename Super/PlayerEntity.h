#pragma once
#include "ConnEntity.h"
#include "TcpConn.h"
#include <string>
#include "Troop.h"
#include <memory>

class Scene;

class PlayerEntity : public loki::ConnEntity
{
	public:
		PlayerEntity(loki::TcpConnPtr conn);
		~PlayerEntity();
		void SetSearchFight(bool fight) { searchFight = fight; }
		bool IsSearchFight() const { return searchFight; }
		uint32_t Accid() const { return accid; }
		void SetAccid(uint32_t a) { accid = a; }

		void EnterScene(std::shared_ptr<Scene> scene);

		void SendCardToMe();

	public:
		uint32_t accid;
		uint32_t charid;
		std::string name;
		Troop troop;
		std::shared_ptr<Scene> scene;
	private:
		bool searchFight;
};
