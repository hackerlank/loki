#pragma once
#include "ConnEntity.h"
#include "TcpConn.h"
#include <string>
#include "Troop.h"
#include <memory>
#include "Super.pb.h"

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

		void EnterScene(Scene* scene);

		void SendCardToMe();

	public:
		uint32_t accid = 0;
		uint32_t charid = 0;
		std::string name;
		Troop troop;
		Scene* scene = nullptr;

		bool online = false;

		void SetOnline(bool on);
		bool IsOnline() const { return online; }

		void DispatchCard(std::shared_ptr<Super::stDispatchCard> msg);
		void NpcMove(std::shared_ptr<Super::stNpcMoveCmd> msg);

		void Offline();

		void Relogin();
		void Login();

		//分主场 客场
		bool host = false;
		std::shared_ptr<NpcData> base;

		//已经出战的npcid, 避免重复出战
		std::set<uint32_t> dispatched;
	private:
		bool searchFight;
};
