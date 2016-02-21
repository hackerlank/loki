#pragma once

#include "GatewayServer.h"

#include "gate.pb.h"
#include "super.pb.h"
#include "record.pb.h"
#include "Session.pb.h"
#include "scene.pb.h"
#include "LoginSessionManager.h"
#include "GatewayTask.h"
#include "GatewayTaskManager.h"

namespace Gate
{
	bool verifyVersion(connection_ptr conn, const std::shared_ptr<Gate::stUserVerifyVerCmd>& msg)
	{
		if (GatewayServer::instance().version() > msg->version())
		{
			LOG(INFO)<<"client version is lower "<<msg->version()<<", current="<<GatewayServer::instance().version();
			return false;
		}
		return true;
	}

	bool verifyAccid(connection_ptr conn, const std::shared_ptr<Gate::stPasswdLogonUserCmd>& msg)
	{
		if (LoginSessionManager::instance().verify(msg->logintempid(), msg->accid()))
		{
			context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
			GatewayTaskPtr task = std::dynamic_pointer_cast<GatewayTask>(context);
			if (task)
			{
				task->set_accid(msg->accid());
				task->set_logintempid(msg->logintempid());

				//设置加密方式

				if (GatewayTaskManager::instance().uniqueAdd(task))
				{
					//TODO: send country info

					//向RecordServer请求人物选择信息
					Record::t_Get_SelectInfo_GateRecord send;
					send.set_accid(msg->accid());
					send.set_ip(conn->get_peer_ip());
					protobuf_codec::send(GatewayServer::instance().recordclient_, &send);
					return true;
				}
				else
				{
					LOG(INFO)<<__func__;
					return false;
				}
			}
		}
		else
		{
			LOG(INFO)<<__func__;
			return false;
		}
	}

	bool checkUserName(connection_ptr conn, const std::shared_ptr<Gate::stCheckNameSelectUserCmd>& msg)
	{
		//doFilter(name) 名字过滤
		auto task = loki::conn_cast<GatewayTask>(conn);

		Record::t_CheckName_GateRecord send;
		send.set_name(msg->name());
		send.set_accid(task->accid());
		protobuf_codec::send(GatewayServer::instance().recordclient_, &send);
		return true;
	}

	bool onCreateCharacter(connection_ptr conn, const std::shared_ptr<Gate::stCreateSelectUserCmd>& msg)
	{
		auto task = loki::conn_cast<GatewayTask>(conn);
		if (task && !task->charInfoFull())
		{
			//check every params
			task->createCharCmd.reset(new Record::t_CreateChar_GateRecord);
			task->createCharCmd->set_accid(task->accid());
			task->createCharCmd->set_name(msg->name());
			task->createCharCmd->set_country(msg->country());
			protobuf_codec::send(GatewayServer::instance().recordclient_, task->createCharCmd);
		}
		return true;
	}

	bool onLoginSelectUser(connection_ptr conn, const std::shared_ptr<Gate::stLoginSelectUserCmd>&msg)
	{
		LOG(INFO)<<__func__<<", login, choose no="<<msg->no();

		auto task = loki::conn_cast<GatewayTask>(conn);
		if (task->getSystemstate() != Systemstate::SYSTEM_STATE_WAIT_UNREG)
		{
			auto userinfo = task->getSelectUserInfo(msg->no());
			if (!userinfo)
			{
				LOG(INFO)<<__func__<<",character not exist, no="<<msg->no();
				return false;
			}
			if (task->tempid() == 0)
			{
				task->set_id(userinfo->id());
				task->set_name(userinfo->name());
				task->set_name(userinfo->name());
				task->set_country(userinfo->country());
				if (GatewayTaskManager::instance().addCountryUser(task))
				{
					task->reg(msg->no());
					return true;
				}
				else
				{
					LOG(INFO)<<__func__<<", .addCountryUser failed accid="<<task->accid();
					return false;
				}
			}
			return false;
		}
		return true;
	}
}
