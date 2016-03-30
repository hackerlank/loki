#pragma once
#include <map>
#include "singleton.h"
#include <ctime>
#include <memory>

struct LoginInfo
{
	LoginInfo(const std::string& k, const uint32_t acc, const time_t t):
		key(k), accid(acc), time(t)
	{
	}
	std::string key;
	uint32_t accid;
	time_t time;
};

class LoginCertification: public Singleton<LoginCertification>
{
private:
	std::map<std::string, std::shared_ptr<LoginInfo> > data;
	uint32_t intervalTimer;
	uint32_t leftTime;
public:
	LoginCertification()
	{
		intervalTimer = 1000;
		leftTime = intervalTimer;
	}
	void Add(const std::string& account, const std::string& key, const uint32_t acc)
	{
		data[account] = std::shared_ptr<LoginInfo>(new LoginInfo(key, acc, time(nullptr)));
	}

	void Remove(const std::string& account)
	{
		data.erase(account);
	}

	std::shared_ptr<LoginInfo> Get(const std::string& acc)
	{
		auto it = data.find(acc);
		if (it != data.end())
			return it->second;
		else
			return std::shared_ptr<LoginInfo>((LoginInfo*)nullptr);
	}
	
	void Update(long delta)
	{
		if (leftTime > delta)
		{
			leftTime -= delta;
			return ;
		}
		leftTime = intervalTimer;
		time_t t = time(nullptr);
		for (auto it = data.begin(); it != data.end();)
		{
			if (it->second->time + 10 < t)
			{
				it = data.erase(it);
			}
			else
				++it;
		}
	}
};
