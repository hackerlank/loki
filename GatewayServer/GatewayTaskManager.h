#pragma once
#include "singleton.h"
#include <unordered_map>
#include <mutex>
#include "GatewayTask.h"
#include <unordered_set>

class GatewayTaskManager: public Singleton<GatewayTaskManager>
{
public:
	~GatewayTaskManager();
	bool uniqueAdd(GatewayTaskPtr& task);
	bool uniqueRemove(GatewayTaskPtr& task);

	GatewayTaskPtr getTaskByID(const uint32_t n);

	uint32_t size();

	bool addCountryUser(const GatewayTaskPtr& task);
	void removeCountryUser(const GatewayTaskPtr& task);
private:
	std::unordered_map<uint32_t, GatewayTaskPtr> tasks_;
	std::mutex mutex_;

	std::unordered_map<uint32_t, std::unordered_set<uint32_t> > countryindex_;
};


