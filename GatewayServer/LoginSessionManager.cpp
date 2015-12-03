#include "LoginSessionManager.h"
#include "mutex.h"
#include "logger.h"

using namespace loki;

void LoginSessionManager::put(const ::Login::t_NewLoginSession &session)
{
	scoped_lock lock(mlock);
	auto it = sessionData.find(session.accid());
	if (it != sessionData.end()) {
		//find, just refresh
		it->second.session.CopyFrom(session);
		it->second.timestamp = microsec_clock::local_time();
	}
	else {
		//can't find, add
		sessionData.insert(std::make_pair(session.accid(), LoginSession(session)));
	}
}

bool LoginSessionManager::verify(const uint64_t loginTempID,const uint32_t accid) {
	bool retval = false;
	scoped_lock lock(mlock);

	auto it = sessionData.find(accid);
	if (it != sessionData.end())
	{
		LOG(INFO)<<__func__<<",loginTempID="<<loginTempID<<",accid="<<accid<<",logintempid="<<it->second.session.logintempid();
		if (loginTempID == it->second.session.logintempid())
		{
			retval = true;
			sessionData.erase(it);
		}
	}
	return retval;
}

void LoginSessionManager::update(const ptime &ct)
{
	if (checkUpdateTime(ct))
	{
		scoped_lock lock(mlock);
		for(auto it = sessionData.begin(); it != sessionData.end();)
		{
			if (ct - it->second.timestamp >= seconds(10))
			{
				LOG(INFO)<<"login time out:accid="<<it->second.session.accid()<<",loginid="<<it->second.session.logintempid()<<",gatewayid="<<it->second.session.gatewayid();
				auto tmp = it;
				it++;
				sessionData.erase(tmp);
			}
			else
				it++;
		}
	}
}

bool LoginSessionManager::checkUpdateTime(const ptime &ct) {
	bool retval = false;
	if (ct >= lastUpdateTime) {
		lastUpdateTime = ct + seconds(1);
		retval = true;
	}
	return retval;
}

LoginSessionManager::~LoginSessionManager()
{
	LOG(INFO)<<__func__;
}
