#pragma once
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "glog/logging.h"
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

			static pointer create(boost::asio::io_service& p)
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

			void Start(bool openPassive = true)
			{
				boost::asio::socket_base::keep_alive option1(true);
				socket_.set_option(option1);

				boost::asio::ip::tcp::no_delay option(true);
				socket_.set_option(option);

				LOG(INFO)<<"Socket started remoteip = "<<GetRemoteIP()<<" remote Port="<<GetRemotePort()<<", "<< (openPassive?"paasive":"active");

				if (connectedHandler)
					connectedHandler(shared_from_this());
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
					Start(false);
					return true;
				}
			}
			void ShutdownRead()
			{
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_receive, ignored_ec);
			}
			void ShutdownWrite()
			{
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec);
			}
			void Shutdown()
			{
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
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

			uint16_t GetRemotePort()
			{
				boost::system::error_code ec;
				boost::asio::ip::tcp::socket::endpoint_type endpoint = socket_.remote_endpoint(ec);
				if (ec)
					return 0;
				else {
					return endpoint.port();
				}

			}

			TcpConnection(boost::asio::io_service& p)
				: socket_(p), data_(nullptr)
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
					Start(false);
				}
				else
					handleError(error, "connect");
			}

			void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					if (!ParseBuffer())	//Failed
					{
						handleError(error, "parse");
					}
					else
					{
						async_read_header();
					}
				}
				else
				{
					handleError(error, "read");
				}
			}
			bool ParseBuffer();
			void handleReadHeader(const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					uint32_t msgLength = msgHeader.flag & 0x0000ffff;
					//LOG(INFO)<<"Header size = "<<msgLength<<", flag = "<<msgHeader.flag;
					if (msgLength > 0)
					{
						msgBuf.resize(msgLength);
						async_read();
					}
				}
				else
				{
					handleError(error, "read");
				}
			}

			void handleError(const boost::system::error_code& error, const std::string& hint)
			{
				if (errorHandler)
					errorHandler(shared_from_this(), error, hint);
				else
					LOG(INFO)<<"Socket Error :"<<hint;
			}

			tcp::socket socket_;

			union MsgHeader{
				char buff[4];
				uint32_t flag;
			} msgHeader;
			std::vector<char> msgBuf;

			std::shared_ptr<encryptor>  encryptor_;
			uint32_t id;
			uint32_t type;
			void* data_;
		public:
			std::function<void (TcpConnection::pointer)> connectedHandler;
			std::function<void (TcpConnection::pointer, const MessagePtr)> msgHandler;
			std::function<void (TcpConnection::pointer, const boost::system::error_code& error, const std::string& hint)> errorHandler;
			uint32_t GetID() const { return id; }
			uint32_t GetType() const { return type; }
			static uint32_t s_id;
			void* GetData() { return data_; }
			void SetData(void* data) { data_ = data; }
	};

};
