#pragma once
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "io_service_pool.h"
#include <boost/noncopyable.hpp>
#include "ServerType.h"
#include "script.h"

namespace loki
{

class service : boost::noncopyable
{
public:
	service(io_service_pool& pool);
	static service * Instance() { return instance_; }
	void setup_signal_callback()
	{
		::signal(SIGPIPE, SIG_IGN);

		//later deal with it
		::signal(SIGHUP, SIG_IGN);

		signals_.add(SIGINT);
		signals_.add(SIGTERM);
		//signals_.add(SIGABRT);
#if defined(SIGQUIT)
		signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
		signals_.async_wait(boost::bind(&service::handle_stop, this));
	}
	virtual ~service() { }
	virtual void handle_stop() 
	{ 
		pool_.stop(); 
	}

	std::shared_ptr<script> get_script() { return script_; }
	lua_State* GetLuaState() { return script_->GetLuaState(); }
	io_service_pool& get_io_service_pool() { return pool_; }
	boost::asio::io_service& get_io_service() { return pool_.get_io_service(); }
	boost::asio::io_service& get_logic_service() { return pool_.get_logic_service(); }
	const bool started() const { return started_; }
	const void set_started(bool b) { started_ = b; }
	const int version() const { return version_; }
	virtual void Run(long delta);
	void StartTimer();
	void StopTimer();
protected:
	void run(const boost::system::error_code& error);
	io_service_pool& pool_;
	boost::asio::signal_set signals_;
	bool started_ = false;
	int version_ = 0;
	std::shared_ptr<script> script_;

	static service* instance_;

	boost::asio::deadline_timer timer_;
	bool timerWaiting;
};

}
