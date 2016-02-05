#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include "glog/logging.h"
#include "io_service_pool.h"

void init_log(const char* cmd)
{
	google::InitGoogleLogging(cmd);
	FLAGS_logbufsecs = 0;
	FLAGS_log_dir = "/home/loki/log/";
}

using boost::asio::ip::tcp;

class tcp_connection
: public boost::enable_shared_from_this<tcp_connection>
{
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;

		static pointer create(boost::asio::io_service& io_service)
		{
			return pointer(new tcp_connection(io_service));
		}

		virtual ~tcp_connection()
		{
			LOG(INFO)<<"~tcp_connection()"<<std::endl;
		}

		tcp::socket& socket()
		{
			return socket_;
		}

		void start()
		{
			async_read_header();
		}
		void async_read_header()
		{
			boost::asio::async_read(socket(), boost::asio::buffer(msgHeader, sizeof(msgHeader)),boost::bind(&tcp_connection::handle_read_header, 
						shared_from_this(),  
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}
		void async_read()
		{
			socket_.async_read_some(boost::asio::buffer(msgBuf),boost::bind(&tcp_connection::handle_read, 
						shared_from_this(),  
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}

	private:
		tcp_connection(boost::asio::io_service& io_service)
			: socket_(io_service)
		{
		}

		void handle_write(const boost::system::error_code& /*error*/,
				size_t /*bytes_transferred*/)
		{
		}

		void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
		{
			if (!error)
			{
				async_read_header();
				//parseMsg();
			}
		}
		void handle_read_header(const boost::system::error_code& error, size_t bytes_transferred)
		{
			if (!error)
			{
				uint32_t size = *(uint32_t*)msgHeader;
				LOG(INFO)<<"Header size = "<<size;
				if (size > 65535) return ;	//error
				msgBuf.resize(size);
				async_read();
			}
		}

		tcp::socket socket_;

		char msgHeader[4];
		std::vector<char> msgBuf;
};

class tcp_server
{
	loki::io_service_pool& service_pool_;
	public:
		tcp_server(loki::io_service_pool& pool, uint16_t port)
			: service_pool_(pool), acceptor_(service_pool_.get_io_service(), tcp::endpoint(tcp::v4(), port))
		{
			start_accept();
		}

	private:
		void start_accept()
		{
			tcp_connection::pointer new_connection =
				tcp_connection::create(service_pool_.get_io_service());

			acceptor_.async_accept(new_connection->socket(),
					boost::bind(&tcp_server::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
		}

		void handle_accept(tcp_connection::pointer new_connection,
				const boost::system::error_code& error)
		{
			if (!error)
			{
				new_connection->start();
			}

			start_accept();
		}

		tcp::acceptor acceptor_;
};

int main(int argc, char** argv)
{
	init_log(argv[0]);
	try
	{
		loki::io_service_pool service_pool(2);
		tcp_server server(service_pool, 11333);
		service_pool.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

