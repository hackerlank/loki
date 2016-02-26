#include <boost/asio.hpp>
#include <iostream>
#include "connection.h"
#include <boost/noncopyable.hpp>
#include <functional>
#include "protobuf_codec.h"
#include "protobuf_dispatcher.h"
#include "Login.pb.h"
#include <boost/bind.hpp>
#include <memory>
#include "logger.h"
#include <boost/lexical_cast.hpp>
#include "gate.pb.h"
#include "compressor_zlib.h"
#include "encryptor_aes.h"
#include <cstdlib>
#include "login_return.h"

using namespace std;
using namespace boost::asio;
using namespace loki;

io_service io;

class login_client;
std::shared_ptr<login_client> cc;

class gate_client : public std::enable_shared_from_this<gate_client>
{
public:
	std::shared_ptr<Login::stServerReturnLoginSuccessCmd> info_;
	gate_client(io_service& io):
		conn_(new connection(io)),
		codec_(std::bind(&protobuf_dispatcher::onProtobufMessage, &dispatcher_, std::placeholders::_1, std::placeholders::_2)),
		dispatcher_([](connection_ptr& conn, const MessagePtr& msg)->bool{ return true;})
	{
		char key[32]= {'a', 'b', 'c'};
		std::shared_ptr<loki::encryptor_aes> aes(new loki::encryptor_aes());
		aes->setkey((const unsigned char*)key,256);
		//conn_->set_encryptor(aes);
		//conn_->set_compressor(std::shared_ptr<loki::compressor>(new loki::compressor_zlib()));

	}
	~gate_client()
	{
		LOG(INFO)<<__func__;
	}
	void init(std::shared_ptr<Login::stServerReturnLoginSuccessCmd> info)
	{
		info_ = info;
		conn_->set_connected_handler(std::bind(&gate_client::handle_connected, shared_from_this(), std::placeholders::_1));
		conn_->set_msg_handler(std::bind(&gate_client::handle_msg, shared_from_this(), std::placeholders::_1));
		conn_->set_error_handler(std::bind(&gate_client::handle_error, shared_from_this(), std::placeholders::_1));

		registerCallback();
	}
	void handle_error(connection_ptr conn)
	{
		LOG(INFO)<<"network error";
		conn->get_io_service().stop();
		dispatcher_.clear();
	}
	void handle_connected(connection_ptr conn)
	{
		cc.reset();
		//版本验证
		Gate::stUserVerifyVerCmd send;
		send.set_version(1);
		codec_.send(conn, &send);

		Gate::stPasswdLogonUserCmd login;
		login.set_logintempid(info_->logintempid());
		login.set_accid(info_->accid());
		login.set_username("loki");
		login.set_password("123456");
		codec_.send(conn, &login);
	}
	void handle_msg(connection_ptr conn)
	{
		codec_.handle_message(conn);
	}
	void registerCallback()
	{
		dispatcher_.registerMsgCallback<Gate::stServerReturnLoginFailedCmd>(std::bind(&gate_client::onLoginFail, this, std::placeholders::_1, std::placeholders::_2));
		dispatcher_.registerMsgCallback<Gate::stUserInfoUserCmd>(std::bind(&gate_client::onUserInfo, this, std::placeholders::_1, std::placeholders::_2));
		dispatcher_.registerMsgCallback<Gate::stCheckNameSelectUserCmd>(std::bind(&gate_client::onRetCheckName, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool onLoginFail(connection_ptr conn, const std::shared_ptr<Gate::stServerReturnLoginFailedCmd>& msg)
	{
		LOG(INFO)<<__func__<<", code="<<msg->returncode();
		switch (msg->returncode())
		{
			case LOGIN_RETURN_USERDATANOEXIST:
				{
					Gate::stCheckNameSelectUserCmd send;
					send.set_name(string("loki") + boost::lexical_cast<string>(rand()%100));
					protobuf_codec::send(conn, &send);
				}
				break;
		}
		return true;
	}
	bool onRetCheckName(connection_ptr conn, const std::shared_ptr<Gate::stCheckNameSelectUserCmd>& msg)
	{
		LOG(INFO)<<__func__<<", name="<<msg->name()<<",code="<<msg->code();
		if (msg->code() == 0)
		{
			//Send create user command
			Gate::stCreateSelectUserCmd send;
			send.set_name(msg->name());
			send.set_country(2);
			protobuf_codec::send(conn, &send);
		}
		else
		{
			//name error
			LOG(INFO)<<__func__<<", error name="<<msg->name()<<",code="<<msg->code();
		}
		return true;
	}

	bool onUserInfo(connection_ptr conn, const std::shared_ptr<Gate::stUserInfoUserCmd>& msg)
	{
		LOG(INFO)<<__func__<<", charinfo size="<<msg->charinfo_size();
		if (msg->charinfo_size() > 0)
		{
			Gate::stLoginSelectUserCmd send;
			send.set_no(rand()%msg->charinfo_size());
			protobuf_codec::send(conn, &send);
		}
		return true;
	}

public:
	connection_ptr conn_;
	protobuf_codec codec_;
	protobuf_dispatcher dispatcher_;
};

std::shared_ptr<gate_client> userclient;

class login_client : boost::noncopyable, public std::enable_shared_from_this<login_client>
{
public:
	login_client(io_service& io):
		conn_(new connection(io)),
		codec_(std::bind(&protobuf_dispatcher::onProtobufMessage, &dispatcher_, std::placeholders::_1, std::placeholders::_2)),
		dispatcher_([](connection_ptr& conn, const MessagePtr& msg)->bool{ return true;})
	{
	}
	~login_client()
	{
		LOG(INFO)<<__func__;
	}
	void init()
	{
		conn_->set_connected_handler(std::bind(&login_client::handle_connected, shared_from_this(), std::placeholders::_1));
		conn_->set_msg_handler(std::bind(&login_client::handle_msg, shared_from_this(), std::placeholders::_1));
		conn_->set_error_handler(std::bind(&login_client::handle_error, shared_from_this(), std::placeholders::_1));

		registerCallback();
	}
	void handle_error(connection_ptr conn)
	{
		LOG(INFO)<<"disconnect from login server";
		//conn->get_io_service().stop();
		dispatcher_.clear();
		//LOG(INFO)<<"login_client count="<<shared_from_this().use_count()<<",conn count="<<conn->shared_from_this().use_count();
	}
	void handle_connected(connection_ptr conn)
	{
		//版本验证
		Login::stUserVerifyVerCmd send;
		send.set_version(1);
		codec_.send(conn, &send);

		//账号登陆
		Login::stUserRequestLoginCmd login;
		login.set_account("loki");
		login.set_passwd("123456");
		login.set_game(1);
		login.set_zone(1);
		codec_.send(conn, &login);
	}
	void handle_msg(connection_ptr conn)
	{
		codec_.handle_message(conn);
	}
	void registerCallback()
	{
		dispatcher_.registerMsgCallback<Login::stServerReturnLoginSuccessCmd>(std::bind(&login_client::onLoginOk, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}
	bool onLoginOk(connection_ptr conn, const std::shared_ptr<Login::stServerReturnLoginSuccessCmd>& msg)
	{
		LOG(INFO)<<msg->GetTypeName()<<" gate ip="<<msg->ip()<<","<<msg->port();
		//连接网关
		userclient->init(msg);
		userclient->conn_->async_connect(msg->ip(), static_cast<uint16_t>(msg->port()));
		return false;
	}

public:
	connection_ptr conn_;
	protobuf_codec codec_;
	protobuf_dispatcher dispatcher_;
};

int main(int argc, char** argv)
{
	InitLog(argv[0]);
	if (argc < 3) 
	{
		cout<<"Usage: ./login_client host port"<<endl;
		return 0;
	}
	LOG(INFO)<<"login_client";
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cc.reset(new login_client(io));
	cc->init();
	cc->conn_->async_connect(argv[1], boost::lexical_cast<uint16_t>(argv[2]));

	userclient.reset(new gate_client(io));

	io.run();

	//google::protobuf::ShutdownProtobufLibrary();
	LOG(INFO)<<"login_client count="<<cc.use_count()<<", gate_client="<<userclient.use_count();
	return 0;
}
