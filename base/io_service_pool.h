#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <thread>
#include <boost/function.hpp>

namespace loki
{

/// A pool of io_service objects.
class io_service_pool : private boost::noncopyable
{
	public:
		/// Construct the io_service pool.
		explicit io_service_pool(std::size_t pool_size);
		~io_service_pool();

		/// Run all io_service objects in the pool.
		void run();
		void start();
		void join();

		/// Stop all io_service objects in the pool.
		void stop();

		/// Get an io_service to use.
		boost::asio::io_service& get_io_service();
		boost::asio::io_service& get_logic_service();

		void post_all(boost::function<void ()> cb);
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
