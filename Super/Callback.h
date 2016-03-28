#pragma once

#include "SuperServer.h"
#include "Super.pb.h"
#include "Login.pb.h"
#include "ServerEntity.h"
#include "LoginCertification.h"

namespace Super
{

bool onStartup_Request(TcpConnPtr conn, std::shared_ptr<Super::t_Startup_Request> msg) {
	if (conn->GetData() != nullptr) {
		LOG(INFO)<<msg->GetTypeName()<<", should be the first message";
		return false;
	}
	ServerEntity* context(new ServerEntity(conn));
	conn->SetData(context);

	context->set_type(msg->type());
	int index = -1;
	ServerList& serverlist = SuperServer::instance().get_serverlist();
	std::unordered_map<uint32_t, ConnEntity*>& sub_conns_ = SuperServer::instance().sub_conns_;
	for (size_t i = 0;i < serverlist.ses_.size(); ++i)
	{
		auto entry = serverlist.ses_[i];
		if (entry->ip() == conn->GetRemoteIP() && entry->type() == msg->type())
		{
			auto it = sub_conns_.find(entry->id());
			if (it != sub_conns_.end()) //already login
				continue;
			index = i;
			break;
		}
	}
	if (index != -1)    //如果还有启动项
	{
		std::vector<int> depend(SuperServer::instance().getDependencyID(msg->type()));  //check dependency
		bool can_boot = true;
		for (size_t i=0;i<depend.size();++i)
		{
			auto it = sub_conns_.find(depend[i]);
			if (it == sub_conns_.end())
			{
				can_boot = false;
				LOG(INFO)<<"type="<<msg->type()<<", server has not started yet, id="<<depend[i];
			}
		}
		if (can_boot)
		{
			context->set_id(serverlist.ses_[index]->id());
			context->set_type(serverlist.ses_[index]->type());
			sub_conns_[context->id()] = context;

			Super::t_Startup_Response send;
			send.set_index(index);

			for (size_t i=0;i<serverlist.ses_.size();++i)
			{
				Super::ServerEntry* en = send.add_entry();
				en->CopyFrom(*(serverlist.ses_[i]));
			}

			conn->SendMessage(&send);
			LOG(INFO)<<__func__<<", ok: type="<<msg->type()<<",name="<<serverlist.ses_[index]->name().c_str();

			//ServerManager::instance().add(context);
			return true;
		}
		else
		{
			LOG(INFO)<<__func__<<", fail:"<<conn->GetRemoteIP()<<", type="<<msg->type();
			return false;
		}
	}
	else
	{
		LOG(INFO)<<__func__<<", fail no more entry:"<<conn->GetRemoteIP()<<",type="<<msg->type();
		return false;
	}
}

bool onUserVerifyVerCmd(TcpConnPtr conn, std::shared_ptr<::Login::stUserVerifyVerCmd> msg)
{
	LOG(INFO)<<__func__;
	return true;
}

bool OnClientLogin(TcpConnPtr conn, std::shared_ptr<Super::stLoginToGame> msg)
{
	auto data = LoginCertification::instance().Get(msg->account());
	if (!data)
	{
		LOG(INFO)<<"not verified by LoginServer";
		return false;
	}
	LoginCertification::instance().Remove(msg->account());
	LOG(INFO)<<"stLoginToGame account = "<<msg->account()<<", key="<<msg->key();
	stLoginGameServerResult send;
	send.set_ret(0);
	conn->SendMessage(&send);
	return true;
}

}
