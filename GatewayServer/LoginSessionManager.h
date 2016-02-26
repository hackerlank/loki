#pragma once

#include <unordered_map>
#include "singleton.h"
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Login.pb.h"
using namespace boost::posix_time;	//ptime

class LoginSessionManager: public Singleton<LoginSessionManager> 
{
public:
	LoginSessionManager() {
		lastUpdateTime = microsec_clock::local_time();
	}
	~LoginSessionManager();

	void put(const ::Login::t_NewLoginSession &session);
	bool verify(const uint64_t loginTempID, const uint32_t accid);
	void update(const ptime &ct);
private:

	ptime lastUpdateTime;

	bool checkUpdateTime(const ptime &ct);

	struct LoginSession {
		::Login::t_NewLoginSession session;
		ptime timestamp;

		LoginSession(const ::Login::t_NewLoginSession &ses) : session(ses) {
			timestamp = microsec_clock::local_time();
		}
	};
	std::unordered_map<uint32_t, LoginSession> sessionData;
	std::mutex mlock;
};
