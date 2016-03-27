#pragma once

#include "SuperServer.h"
#include "Login.pb.h"
#include "Super.pb.h"
#include <memory>

namespace Super
{
	bool OnPreLoginServer(TcpConnPtr conn, std::shared_ptr<::Login::t_NewSession_Session> msg)
	{
		/*
		Super::t_NewSession_Gateway send;
		send.mutable_session()->CopyFrom(msg->session());
		//从网管返回时需要这个flclient
		send.mutable_session()->set_loginid(conn->get_id());
		//转发到网关
		ServerManager::instance().broadcastByID(send.session().gatewayid(), &send);
		*/
		//TODO: check if user is already in game
		LOG(INFO)<<__func__<<"accid = "<<msg->session().accid()<<", key="<<msg->session().key();
		conn->SendMessage(msg);
		return true;
	}

	/*
	bool onPlayerLogin(connection_ptr conn, const std::shared_ptr<::Login::PlayerLogin>& msg)
	{
		return true;
	}
	*/

	//连接平台成功
	bool OnRetZoneLogin(TcpConnPtr conn, std::shared_ptr<::Login::t_LoginFL_OK> msg)
	{
		SuperServer::instance().game_ = msg->game();
		SuperServer::instance().zone_ = msg->zone();
		SuperServer::instance().name_ = msg->name();
		LOG(INFO)<<__func__<<", ok: game="<<msg->game()<<",zone="<<msg->zone()<<",name="<<msg->name();

		Login::t_GYList_FL gate;
		gate.set_serverid(1);
		gate.set_ip(SuperServer::instance().ip_);
		gate.set_port(SuperServer::instance().port_);
		gate.set_num_online(100);
		gate.set_version(100);
		gate.set_net_type(10);
		conn->SendMessage(&gate);
		return true;
	}   
}
