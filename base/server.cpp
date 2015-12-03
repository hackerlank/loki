#include "server.h"
#include <boost/bind.hpp>
#include "logger.h"

namespace loki
{

server::server(const std::string& address, 
		const std::string& port, 
		io_service_pool& pool)
	:io_service_pool_(pool), 
	acceptor_(io_service_pool_.get_io_service())
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

void server::start_accept() 
{
	connection_ptr conn(new connection(io_service_pool_.get_io_service()));
	if (conn)
	{
		conn->set_connected_handler(connected_handler_);
		conn->set_msg_handler(msg_handler_);
		conn->set_error_handler(error_handler_);

		acceptor_.async_accept(conn->socket(), 
				boost::bind(&server::handle_accept,
					this, conn, boost::asio::placeholders::error));
	}
}

void server::handle_accept(connection_ptr conn, const boost::system::error_code& e)
{
	if (!e) 
	{
		LOG(INFO)<<__func__<<",connectionid="<<conn->get_id()<<",established ip="<<conn->get_peer_ip()<<",port"<<conn->get_peer_port();
		conn->get_io_service().dispatch(boost::bind(&connection::start, conn));
	}
	else
	{
		LOG(INFO)<<__func__<<", error_msg="<<e.message()<<",error_value="<<e.value();
	}

	start_accept();
}

}
