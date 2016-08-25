#include <boost/asio.hpp>
#include <iostream>
#include "TcpConn.h"
#include <boost/noncopyable.hpp>
#include <functional>
#include <boost/bind.hpp>
#include <memory>
#include "logger.h"
#include <boost/lexical_cast.hpp>
#include <cstdlib>

using namespace std;
using namespace boost::asio;
using namespace loki;

class Client
{
public:
	TcpConnPtr conn;
	std::string senddata;
public:
	Client(io_service& io):conn(new TcpConnection(io)), senddata(8192, 'a')
	{
		conn->connectedHandler = std::bind(&Client::OnConnected, this, std::placeholders::_1);
		conn->msgHandler = std::bind(&Client::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		conn->errorHandler = std::bind(&Client::OnError, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}

	void OnConnected(TcpConnPtr conn)
	{
		LOG(INFO)<<__func__;
		conn->SendMessage(senddata.data(), senddata.size());
	}
	void OnError(TcpConnPtr conn, const boost::system::error_code& err, const string& hint)
	{
		LOG(INFO)<<__func__<<": "<<hint;
	}
	void OnMessage(TcpConnPtr conn, const char* msg, const uint32_t size)
	{
		//LOG(INFO)<<std::string(msg, size);
		count += size;
		conn->SendMessage(msg, size);
		/*
		if (count > 1024* 100)
		{
			LOG(INFO)<<"Need exit";
			conn->socket().close();
		}
		else
		{
		}
		*/
	}

private:
	uint32_t count = 0;
};

int main(int argc, char** argv)
{
	LoggerHelper::InitLog(argv[0]);
	if (argc < 3) 
	{
		cout<<"Usage: ./login_client host port"<<endl;
		return 0;
	}
	LOG(INFO)<<"login_client";
	//GOOGLE_PROTOBUF_VERIFY_VERSION;
	io_service io;

	std::shared_ptr<Client> client(new Client(io));
	client->conn->AsyncConnect(argv[1], boost::lexical_cast<uint16_t>(argv[2]));

	io.run();

	//google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
