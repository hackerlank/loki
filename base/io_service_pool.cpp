#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "io_service_pool.h"
#include "logger.h"

namespace loki
{

io_service_pool::io_service_pool(std::size_t pool_size) 
	: next_io_service_(1)
{
	running_ = false;
	if (pool_size == 0)
		throw std::runtime_error("io_service_pool size is 0");

	for (std::size_t i = 0; i < pool_size; ++i)
	{
		io_service_ptr io_service(new boost::asio::io_service);
		work_ptr work(new boost::asio::io_service::work(*io_service));
		io_services_.push_back(io_service);
		work_.push_back(work);
	}
}

io_service_pool::~io_service_pool()
{
	LOG(INFO)<<__func__;
}

void io_service_pool::post_all(boost::function<void ()> handler)
{
	for (std::size_t i = 0;i< io_services_.size(); ++i)
	{
		io_services_[i]->post(handler);
	}
}

void io_service_pool::run() {
	start();
	join();
}

void io_service_pool::start() {
	threads.clear();
	for (std::size_t i = 0; i < io_services_.size(); ++i)
	{
		boost::shared_ptr<std::thread> thread(
				new std::thread(
					boost::bind(&boost::asio::io_service::run, 
						io_services_[i])));
		threads.push_back(thread);
	}
	running_ = true;
}

void io_service_pool::join() {
	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i)
		threads[i]->join();
}

void io_service_pool::stop()
{
	// Explicitly stop all io_services.
	for (std::size_t i = 0; i < io_services_.size(); ++i)
		io_services_[i]->stop();
	LOG(INFO)<<__func__;
}

boost::asio::io_service& io_service_pool::get_io_service()
{
	if (io_services_.size() == 1)
		return *io_services_[0];
		
	// Use a round-robin scheme to choose the next io_service to use.
	boost::asio::io_service& io_service = *io_services_[next_io_service_];
	++next_io_service_;
	if (next_io_service_ == io_services_.size())
		next_io_service_ = 1;
	return io_service;
}

boost::asio::io_service& io_service_pool::get_logic_service()
{
	assert (!io_services_.empty());
	
	return *io_services_[0];
}

}
