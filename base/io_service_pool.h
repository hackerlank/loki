#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <thread>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace loki
{

/// A pool of io_service objects.
class io_service_pool : private boost::noncopyable
{
	public:
		/// Construct the io_service pool.
		explicit io_service_pool(std::size_t pool_size) 
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

		/// Run all io_service objects in the pool.
		void run() {
			start();
			join();
		}

		void start() {
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

		void join() {
			// Wait for all threads in the pool to exit.
			for (std::size_t i = 0; i < threads.size(); ++i)
				threads[i]->join();
		}

		/// Stop all io_service objects in the pool.
		void stop()
		{
			// Explicitly stop all io_services.
			for (std::size_t i = 0; i < io_services_.size(); ++i)
				io_services_[i]->stop();
		}

		/// Get an io_service to use.
		boost::asio::io_service& get_io_service()
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
		boost::asio::io_service& get_logic_service()
		{
			assert (!io_services_.empty());

			return *io_services_[0];
		}

		void post_all(boost::function<void ()> handler)
		{
			for (std::size_t i = 0;i< io_services_.size(); ++i)
			{
				io_services_[i]->post(handler);
			}
		}
	private:
		typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
		typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

		/// The pool of io_services.
		std::vector<io_service_ptr> io_services_;

		/// The work that keeps the io_services running.
		std::vector<work_ptr> work_;

		/// The next io_service to use for a connection.
		std::size_t next_io_service_;

		// Create a pool of threads to run all of the io_services.
		std::vector<boost::shared_ptr<std::thread> > threads;

		bool running_;
};

}
