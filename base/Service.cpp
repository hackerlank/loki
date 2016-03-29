#include "Service.h"
#include "logger.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

namespace loki
{
	Service* Service::instance_ = nullptr;

	Service::Service(io_service_pool& pool) :
		pool_(pool),
		signals_(pool_.get_io_service()),
		timer_(pool_.get_logic_service())
	{
		instance_ = this;
		LOG(INFO)<<"Service create";
		timerWaiting = false;
	}

	void Service::StartTimer()
	{
		timer_.expires_from_now(boost::posix_time::milliseconds(30));
		timer_.async_wait(boost::bind(&Service::run, this, _1));
		timerWaiting = true;
	}

	void Service::run(const boost::system::error_code& error)
	{
		timerWaiting = false;
		if (error)
		{
			LOG(INFO)<<"Service::Run stoped";
			return;
		}
		boost::posix_time::ptime curTime = boost::posix_time::microsec_clock::local_time();
		static boost::posix_time::ptime lastTime = curTime;
		Run((curTime - lastTime).total_milliseconds());
		lastTime = curTime;

		StartTimer();
	}

	void Service::Run(long milli)
	{
		//LOG(INFO)<<"Delta = "<<milli;
	}

	void Service::StopTimer()
	{
		LOG(INFO)<<__func__;
		if (timerWaiting)
		{
			timer_.cancel();
			timerWaiting = false;
		}
	}
};
