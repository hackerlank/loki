#pragma once

#include "SuperServer.h"
#include "Login.pb.h"
#include "Super.pb.h"
#include <memory>

namespace Super
{
	/*
	bool on_NewSession_Session(connection_ptr conn, const std::shared_ptr<::Login::t_NewSession_Session>& msg)
	{
		Super::t_NewSession_Gateway send;
		send.mutable_session()->CopyFrom(msg->session());
		//从网管返回时需要这个flclient
		send.mutable_session()->set_loginid(conn->get_id());
		//转发到网关
		ServerManager::instance().broadcastByID(send.session().gatewayid(), &send);
		return true;
	}
	bool onPlayerLogin(connection_ptr conn, const std::shared_ptr<::Login::PlayerLogin>& msg)
	{
		return true;
	}
	*/

	//连接平台成功，广播网关请求网关信息
	bool OnRetZoneLogin(TcpConnPtr conn, std::shared_ptr<::Login::t_LoginFL_OK> msg)
	{
		SuperServer::instance().game_ = msg->game();
		SuperServer::instance().zone_ = msg->zone();
		SuperServer::instance().name_ = msg->name();
		LOG(INFO)<<__func__<<", ok: game="<<msg->game()<<",zone="<<msg->zone()<<",name="<<msg->name();

		/*
		Super::t_RQGYList_Gateway send;
		//转发到网关
		ServerManager::instance().broadcastByType(GATEWAYSERVER, &send);
		*/
		return true;
	}   
}
