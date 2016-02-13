#pragma once
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "glog/logging.h"
#include "io_service_pool.h"
#include "encryptor.h"
#include <cassert>
#include <memory>		//for std::shared_ptr std::enable_shared_from_this

#include <google/protobuf/message.h>

namespace loki
{

	typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

	using boost::asio::ip::tcp;

	class tcp_connection;
	typedef std::shared_ptr<tcp_connection> TcpConnPtr;

	class tcp_connection
		: public std::enable_shared_from_this<tcp_connection>
	{
		public:
			typedef std::shared_ptr<tcp_connection> pointer;

			static pointer create(loki::io_service_pool& p)
			{
				return pointer(new tcp_connection(p));
			}

			virtual ~tcp_connection()
			{
				LOG(INFO)<<"~tcp_connection() id="<<GetID()<<std::endl;
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
				boost::asio::async_read(socket(), boost::asio::buffer(msgHeader.buff, sizeof(msgHeader.buff)),boost::bind(&tcp_connection::handle_read_header, 
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

			void close()
			{
				socket_.close();
			}

		private:
			tcp_connection(loki::io_service_pool& p)
				: socket_(p.get_io_service()), pool(p)
			{
				id = ++ s_id;
			}

			void handle_write(const boost::system::error_code& /*error*/,
					size_t /*bytes_transferred*/)
			{
			}

			void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					if (!ParseBuffer())	//Failed
					{
					}
					else
					{
						async_read_header();
					}
				}
			}
			bool ParseBuffer();
			void handle_read_header(const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					uint32_t msgLength = msgHeader.flag & 0x0000ffff;
					LOG(INFO)<<"Header size = "<<msgLength<<", flag = "<<msgHeader.flag;
					if (msgLength > 0)
					{
						msgBuf.resize(msgLength);
						async_read();
					}
				}
			}

			tcp::socket socket_;

			union MsgHeader{
				char buff[4];
				uint32_t flag;
			} msgHeader;
			std::vector<char> msgBuf;

			loki::io_service_pool& pool;
			std::shared_ptr<encryptor>  encryptor_;
			uint32_t id;
		public:
			std::function<void (tcp_connection::pointer, const MessagePtr)> msgHandler;
			uint32_t GetID() const { return id; }
			static uint32_t s_id;
	};

};
