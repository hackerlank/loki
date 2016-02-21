#pragma once

#include "GatewayServer.h"

#include "gate.pb.h"
#include "super.pb.h"
#include "record.pb.h"
#include "Session.pb.h"
#include "scene.pb.h"
#include "LoginSessionManager.h"
#include "login_return.h"

namespace Gate
{
	bool onRQGYList_Gateway(connection_ptr conn, const std::shared_ptr<Super::t_RQGYList_Gateway>& msg)
	{
		GatewayServer::instance().notify_login_server();
		return true;
	}

	//平台转发来的玩家登录请求
	bool onNewSession_Gateway(connection_ptr conn, const std::shared_ptr<Super::t_NewSession_Gateway>& msg)
	{
		LOG(INFO)<<__func__<<",logintempid="<<msg->session().logintempid()<<",accid="<<msg->session().accid();
		LoginSessionManager::instance().put(msg->session());
		protobuf_codec::send(GatewayServer::instance().superclient_, msg);
		return true;
	}

	bool onRetSelectUserInfo(connection_ptr conn, const std::shared_ptr<Record::t_Ret_SelectInfo_GateRecord>& msg)
	{
		LOG(INFO)<<__func__<<",accid="<<msg->accid()<<",info_size="<<msg->info_size();
		auto task = GatewayTaskManager::instance().getTaskByID(msg->accid());
		if (task && task->tempid() == 0)
		{
			task->setSelectUserInfo(msg);
			if (msg->info_size() == 0)	//no character
			{
				Gate::stServerReturnLoginFailedCmd send;
				send.set_returncode(LOGIN_RETURN_USERDATANOEXIST);
				protobuf_codec::send(task->connection(), &send);
				task->selectState();
			}
			else
			{
				task->refreshCharInfo();
			}
		}
		return true;
	}

	/*
	   from Record
	 */
	bool onRetCheckName(connection_ptr conn, const std::shared_ptr<Record::t_CheckName_Return_GateRecord>& msg)
	{
		auto task = GatewayTaskManager::instance().getTaskByID(msg->accid());
		if (task)
		{
			Gate::stCheckNameSelectUserCmd send;
			send.set_name(msg->name());
			send.set_code(msg->code());
			protobuf_codec::send(task->connection(), &send);
		}
		return true;
	}

	bool onRetCreateChar(connection_ptr conn, const std::shared_ptr<Record::t_CreateChar_Return_GateRecord>& msg)
	{
		auto task = GatewayTaskManager::instance().getTaskByID(msg->accid());
		if (task)
		{
			if (msg->retcode() == 1 && msg->has_charinfo())	//ok
			{
				task->addSelectUserInfo(msg->charinfo()); 
				task->refreshCharInfo();
			}
			else
			{
				//error
				Gate::stServerReturnLoginFailedCmd send;
				send.set_returncode(LOGIN_RETURN_CHARNAMEREPEAT);
				task->selectState();
				protobuf_codec::send(task->connection(), &send);
			}
		}
		return true;
	}

	/*
	   from Scene
	 */
	bool onFresh_MapIndex(connection_ptr conn, const std::shared_ptr<SceneCmd::t_fresh_MapIndex>& msg)
	{
		for (size_t i=0;i<msg->mps_size(); ++i)
		{
		}
		return true;
	}
}
