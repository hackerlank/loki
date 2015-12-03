#include "GYListManager.h"

bool GYListManager::put(const GameZone_t & gameZone, const t_GYList_FL_ptr& gy)
{
	loki::scoped_lock lock(mutex_);
	auto hps = gyData_.equal_range(gameZone);			//game zone 
	for (auto it = hps.first; it != hps.second; ++it)
	{
		if (it->second->serverid() == gy->serverid())	//gateway server id, a zone can have several gateways
		{
			it->second = gy;
			return true;
		}
	}
	gyData_.insert(std::make_pair(gameZone, gy));
	return true;
}

void GYListManager::disableAll(const GameZone_t& gameZone)
{
	loki::scoped_lock lock(mutex_);
	gyData_.erase(gameZone);
}

//查找人数最少的网关
t_GYList_FL_ptr GYListManager::getAvl(const GameZone_t& gameZone)
{
	loki::scoped_lock lock(mutex_);

	t_GYList_FL_ptr ret, tmp;

	auto hps = gyData_.equal_range(gameZone);
	for (auto it = hps.first; it != hps.second; ++it)
	{
		tmp = it->second;
		if ( !ret || ret->num_online() >= tmp->num_online())
		{
			ret = tmp;
		}
	}
	return ret;
}

bool GYListManager::verifyVersion(const GameZone_t& gameZone, uint32_t verify_client_version, uint32_t & retcode)
{
	loki::scoped_lock lock(mutex_);
	return true;
}

uint32_t GYListManager::getOnline()
{
	uint32_t n = 0;
	loki::scoped_lock lock(mutex_);

	for (auto it = gyData_.begin(); it != gyData_.end(); ++it)
	{
		n += it->second->num_online();
	}
	return n;
}
