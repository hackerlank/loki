#pragma once

#include "Super.pb.h"
#include <vector>
#include <map>
#include <memory>

typedef std::shared_ptr<Super::ServerEntry> ServerEntryPtr ;

class ServerList
{
public:
	ServerList();
	std::vector<int> getIDByType(const uint32_t) const;

	ServerEntryPtr getEntryByType(const uint32_t) const;
	std::vector<ServerEntryPtr> getEntrysByType(const uint32_t) const;
	void addEntry(ServerEntryPtr);

	std::vector<ServerEntryPtr> ses_;
};
