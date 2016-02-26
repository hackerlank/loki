#pragma once

#include "LoginServer.h"
#include "Login.pb.h"
#include "GYListManager.h"
#include "login_return.h"
#include "db.h"
#include "ServerManager.h"

namespace Login
{
	//verifyConn
	bool onZoneLogin(connection_ptr conn, const std::shared_ptr<Login::ReqZoneLogin>& msg)
	{
		ServerTaskPtr context(boost::any_cast<ServerTaskPtr>(conn->get_context()));
		if (!context) 
		{
			return false;
		}
		if (context->state() != 0)
		{
			LOG(INFO)<< msg->GetTypeName()<<" should be the first message";
			return false;
		}
		else 
		{
			//check ip, port and server openport
			const std::shared_ptr<ACLZone> zone = LoginServer::instance().getAcl(conn->get_peer_ip(), msg->openport());
			if (zone)
			{
				context->set_id(zone->gamezone.id);	//跟区下面的服务器使用不同的方式表示id
				context->set_zoneinfo(zone);
				LOG(INFO)<<__func__<<", ok:"<<conn->get_peer_info()<<",openport="<<msg->openport();

				Login::t_LoginFL_OK ret;
				ret.set_game(zone->gamezone.game);
				ret.set_zone(zone->gamezone.zone);
				ret.set_name(zone->name);
				protobuf_codec::send(conn, &ret);
				ServerManager::instance().add(context);
				return true;
			}
			else
			{
				LOG(INFO)<<__func__<<", fail:"<<conn->get_peer_info()<<",openport="<<msg->openport();
				return false;
			}
		}
	}

	//receive gateway info
	bool onGYList_FL(connection_ptr conn, const std::shared_ptr<Login::t_GYList_FL>& msg)
	{
		ServerTaskPtr context(boost::any_cast<ServerTaskPtr>(conn->get_context()));
		GYListManager::instance().put(context->gamezone(), msg);	
		LOG(INFO)<<__func__<<", serverid="<<msg->serverid()<<",ip="<<msg->ip()<<",port="<<msg->port()<<",num_online="<<msg->num_online()<<",version="<<msg->version();
		return true;
	}

	bool onNewSession_Session(connection_ptr conn, const std::shared_ptr<Login::t_NewSession_Session>& msg)
	{
		//TODO
		stServerReturnLoginSuccessCmd send;
		send.set_accid(msg->session().accid());
		send.set_ip(msg->session().ip());
		send.set_port(msg->session().port());
		send.set_logintempid(msg->session().logintempid());
		//send to user
		LoginServer::instance().broadcastByID(msg->session().logintempid(), &send);
		return true;
	}
}
