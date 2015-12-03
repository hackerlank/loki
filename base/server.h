#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include "connection.h"
#include "io_service_pool.h"
//#include <boost/function.hpp>
#include <functional>	//bind function

namespace loki
{

class server : private boost::noncopyable
{
public:
	/// Construct the server to listen on the specified TCP address and port, and
	/// serve up files from the given directory.
	explicit server(const std::string& address, 
			const std::string& port, 
			io_service_pool& pool);

	void set_connected_handler(std::function<void (connection_ptr)> cb)
	{
		connected_handler_ = cb;
	}
	void set_msg_handler(std::function<void (connection_ptr)> cb)
	{
		msg_handler_ = cb;
	}
	void set_error_handler(std::function<void (connection_ptr)> cb)
	{
		error_handler_ = cb;
	}

	/// Initiate an asynchronous accept operation.
	void start_accept();
private:

	/// Handle completion of an asynchronous accept operation.
	void handle_accept(connection_ptr new_task, const boost::system::error_code& e);

	//for acceptor
	/// The pool of io_service objects used to perform asynchronous operations.
	io_service_pool& io_service_pool_;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor acceptor_;

	//connection handler
	std::function<void (connection_ptr)> connected_handler_;
	std::function<void (connection_ptr)> msg_handler_;
	std::function<void (connection_ptr)> error_handler_;
};

};
