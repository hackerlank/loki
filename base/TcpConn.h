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
#include <boost/lexical_cast.hpp>
#include <google/protobuf/message.h>

namespace loki
{
	typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

	using boost::asio::ip::tcp;

	class TcpConnection;
	typedef std::shared_ptr<TcpConnection> TcpConnPtr;

	class TcpConnection
		: public std::enable_shared_from_this<TcpConnection>
	{
		public:
			typedef std::shared_ptr<TcpConnection> pointer;

			static pointer create(loki::io_service_pool& p)
			{
				return pointer(new TcpConnection(p));
			}

			virtual ~TcpConnection()
			{
				LOG(INFO)<<"~TcpConnection() id="<<GetID()<<std::endl;
			}

			tcp::socket& socket()
			{
				return socket_;
			}

			void Start()
			{
				boost::asio::socket_base::keep_alive option1(true);
				socket_.set_option(option1);

				boost::asio::ip::tcp::no_delay option(true);
				socket_.set_option(option);

				async_read_header();
			}
			void async_read_header()
			{
				boost::asio::async_read(socket(), boost::asio::buffer(msgHeader.buff, sizeof(msgHeader.buff)),boost::bind(&TcpConnection::handleReadHeader, 
							shared_from_this(),  
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
			}
			void async_read()
			{
				socket_.async_read_some(boost::asio::buffer(msgBuf),boost::bind(&TcpConnection::handle_read, 
							shared_from_this(),  
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
			}

			void close()
			{
				socket_.close();
			}
			void AsyncConnect(const std::string& host, const uint16_t port)
			{
				using namespace boost::asio::ip;
				tcp::resolver resolver(socket_.get_io_service());
				tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
				tcp::resolver::iterator iterator = resolver.resolve(query);

				boost::asio::async_connect(socket_, iterator, boost::bind(&TcpConnection::handleConnect, shared_from_this(), boost::asio::placeholders::error));
			}   
			bool Connect(const std::string& host, const uint16_t port)
			{
				boost::system::error_code ec;
				boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host.c_str()), port);
				socket_.connect(endpoint, ec);
				if (ec) return false;
				else {
					Start();
					return true;
				}
			}

			const std::string GetRemoteIP()
			{
				boost::system::error_code ec;
				boost::asio::ip::tcp::socket::endpoint_type endpoint = socket_.remote_endpoint(ec);
				if (ec)
					return "";
				else {
					return endpoint.address().to_string();
				}
			}

			TcpConnection(loki::io_service_pool& p)
				: socket_(p.get_io_service()), pool(p), data_(nullptr)
			{
				id = ++ s_id;
			}
			void SendMessage(const MessagePtr msg);
			void SendMessage(const google::protobuf::Message* msg);

		private:
			void handleWrite(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);

			void handleConnect(const boost::system::error_code& error)
			{
				if (!error) {
					//async connect ok
					Start();
				}
				if (connectedHandler)
					connectedHandler(shared_from_this(), error);
			}

			void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					if (!ParseBuffer())	//Failed
					{
						LOG(INFO)<<"Parse Buffer Error";
						handleError(error);
					}
					else
					{
						async_read_header();
					}
				}
				else
				{
					handleError(error);
				}
			}
			bool ParseBuffer();
			void handleReadHeader(const boost::system::error_code& error, size_t bytes_transferred)
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
				else
				{
					handleError(error);
				}
			}

			void handleError(const boost::system::error_code& error)
			{
				if (errorHandler)
					errorHandler(shared_from_this(), error);
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
			void* data_;
		public:
			std::function<void (TcpConnection::pointer, const boost::system::error_code& error)> connectedHandler;
			std::function<void (TcpConnection::pointer, const MessagePtr)> msgHandler;
			std::function<void (TcpConnection::pointer, const boost::system::error_code& error)> errorHandler;
			uint32_t GetID() const { return id; }
			static uint32_t s_id;
			void* GetData() { return data_; }
			void SetData(void* data) { data_ = data; }
	};

};
