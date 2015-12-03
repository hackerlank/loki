#include "ServerList.h"
#include "logger.h"
#include "ServerType.h"
#include <algorithm>

ServerList::ServerList()
{
}

std::vector<int> ServerList::getIDByType(const uint32_t type) const
{
	std::vector<int> ids;
	for (size_t i=0;i<ses_.size(); ++i)
	{
		if (ses_[i]->type() == type)
			ids.push_back(ses_[i]->id());
	}
	return ids;
}

ServerEntryPtr ServerList::getEntryByType(const uint32_t type) const
{
	ServerEntryPtr entry;
	for (size_t i=0;i<ses_.size(); ++i)
	{
		if (ses_[i]->type() == type)
			return ses_[i];
	}
	return entry;
}

void ServerList::addEntry(ServerEntryPtr entry)
{
	ses_.push_back(entry);
}

std::vector<ServerEntryPtr> ServerList::getEntrysByType(const uint32_t type) const
{
	std::vector<ServerEntryPtr> entrys;
	for (size_t i=0;i<ses_.size(); ++i)
	{
		if (ses_[i]->type() == type)
			entrys.push_back(ses_[i]);
	}
	return entrys;
}
