#include "GatewayTaskManager.h"
#include "mutex.h"
#include "logger.h"

using namespace loki;
GatewayTaskManager::~GatewayTaskManager()
{
	LOG(INFO)<<__func__;
}

bool GatewayTaskManager::uniqueAdd(GatewayTaskPtr& task)
{
	scoped_lock lock(mutex_);
	auto it = tasks_.find(task->accid());
	if (it != tasks_.end())
		return false;
	else
	{
		tasks_.insert(std::make_pair(task->accid(), task));
		return true;
	}
}

bool GatewayTaskManager::uniqueRemove(GatewayTaskPtr& task)
{
	scoped_lock lock(mutex_);
	auto it = tasks_.find(task->accid());
	bool a = false;
	if (it != tasks_.end())
	{
		tasks_.erase(it);
		a = true;
	}
	LOG(INFO)<<__func__<<", accid="<<task->accid()<<","<<a?"ok":"falied";
	return true;
}

uint32_t GatewayTaskManager::size()
{
	scoped_lock lock(mutex_);
	return tasks_.size();
}

GatewayTaskPtr GatewayTaskManager::getTaskByID(const uint32_t n)
{
	scoped_lock lock(mutex_);
	auto it = tasks_.find(n);
	if (it != tasks_.end())
		return it->second;
	else
		return GatewayTaskPtr();
}

bool GatewayTaskManager::addCountryUser(const GatewayTaskPtr& task)
{
	bool ret = countryindex_[task->country()].insert(task->accid()).second;
	return ret;
}

void GatewayTaskManager::removeCountryUser(const GatewayTaskPtr& task)
{
	auto it = countryindex_.find(task->country());
	if (it != countryindex_.end())
	{
		it->second.erase(task->accid());
	}
}
