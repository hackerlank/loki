#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include "glog/logging.h"
#include <fstream>

void init_log(const char* cmd)
{
	FLAGS_logbufsecs = 0;
	google::InitGoogleLogging(cmd);
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
			async_read();
		}
		void async_read()
		{
			socket_.async_read_some(boost::asio::buffer(read_buf_),boost::bind(&tcp_connection::handle_read, 
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
				if (get_file())
				{
					if (*file_)
					{
						(*file_)<<std::string(&read_buf_[0], bytes_transferred);
						(*file_).flush();
						async_read();
					}
				}
			}
		}
		std::shared_ptr<std::ofstream> get_file()
		{
			if (!file_)
			{
				boost::asio::ip::tcp::socket::endpoint_type endpoint = socket_.remote_endpoint();
				std::string filename = endpoint.address().to_string() + "_" + boost::lexical_cast<std::string>(endpoint.port());
				file_.reset(new std::ofstream(std::string("/tmp/") + filename));
			}
			return file_;
		}

		std::shared_ptr<std::ofstream> file_;
		tcp::socket socket_;
		char read_buf_[1024];
};

class tcp_server
{
	public:
		tcp_server(boost::asio::io_service& io_service)
			: acceptor_(io_service, tcp::endpoint(tcp::v4(), 11334))
		{
			start_accept();
		}

	private:
		void start_accept()
		{
			tcp_connection::pointer new_connection =
				tcp_connection::create(acceptor_.get_io_service());

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
		boost::asio::io_service io_service;
		tcp_server server(io_service);
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

