#pragma once

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "io_service_pool.h"
#include "TcpConn.h"

using boost::asio::ip::tcp;
namespace loki
{

class TcpServer
{
	public:
		TcpServer(loki::io_service_pool& pool, const std::string& address, const std::string& port)
			: service_pool_(pool), acceptor_(service_pool_.get_io_service())
		{
			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
			boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
			boost::asio::ip::tcp::resolver::query query(address, port);
			boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

			acceptor_.open(endpoint.protocol());
			acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor_.bind(endpoint);
			acceptor_.listen();
		}

		void start_accept()
		{
			tcp_connection::pointer new_connection = tcp_connection::create(service_pool_);

			new_connection->msgHandler = msgHandler;

			acceptor_.async_accept(new_connection->socket(),
					boost::bind(&TcpServer::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
		}

	private:
		void handle_accept(tcp_connection::pointer new_connection,
				const boost::system::error_code& error)
		{
			if (!error)
			{
				new_connection->start();
			}

			start_accept();
		}

		loki::io_service_pool& service_pool_;
		tcp::acceptor acceptor_;

	public:
		std::function<void (TcpConnPtr, const MessagePtr)> msgHandler;
};

}
