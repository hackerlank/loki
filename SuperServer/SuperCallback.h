#pragma once

#include "SuperServer.h"
#include "Login.pb.h"
#include "Super.pb.h"
#include "ServerManager.h"
#include <memory>

namespace Super
{
	//{ from sub server
	//sub server login to SuperServer
	bool onStartup_Request(connection_ptr conn, const std::shared_ptr<Super::t_Startup_Request>& msg)
	{
		context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
		if (!context)
		{
			return false;
		}
		if (context->state() != 0)
		{
			LOG(ERROR)<<msg->GetTypeName()<<", should be the first message";
			return false;
		}
		else
		{
			context->set_type(msg->type());
			int index = -1;
			ServerList& serverlist = SuperServer::instance().get_serverlist();
			std::unordered_map<uint32_t, context_ptr>& sub_conns_ = SuperServer::instance().sub_conns_;
			for (size_t i = 0;i < serverlist.ses_.size(); ++i)
			{
				auto entry = serverlist.ses_[i];
				if (entry->ip() == conn->get_peer_ip() && entry->type() == msg->type())
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

					protobuf_codec::send(conn, &send);
					LOG(INFO)<<__func__<<", ok: "<<conn->get_peer_info()<<", type="<<msg->type()<<",name="<<serverlist.ses_[index]->name().c_str();

					ServerManager::instance().add(context);
					return true;
				}
				else
				{
					LOG(INFO)<<__func__<<", fail:"<<conn->get_peer_info()<<", type="<<msg->type();
					return false;
				}
			}
			else
			{
				LOG(INFO)<<__func__<<", fail no more entry:"<<conn->get_peer_info()<<",type="<<msg->type();
				return false;
			}
		}
	}

	//from Gateway 收到网关信息，发送到login
	bool onGYList_Gateway(connection_ptr conn, const std::shared_ptr<Super::t_GYList_Gateway>& msg)
	{
		::Login::t_GYList_FL send;
		send.CopyFrom(msg->gateinfo());
		if (SuperServer::instance().flclient_->is_connected())
			protobuf_codec::send(SuperServer::instance().flclient_, &send);
		LOG(INFO)<<__func__;
		return true;
	}

	//gate->super super->login
	bool on_NewSession_Gateway(connection_ptr conn, const std::shared_ptr<Super::t_NewSession_Gateway>& msg)
	{
		::Login::t_NewSession_Session send;
		send.mutable_session()->CopyFrom(msg->session());
		//TODO: get flclient by loginid, right now there is only one flclient.
		if (SuperServer::instance().flclient_->is_connected())
			protobuf_codec::send(SuperServer::instance().flclient_, &send);
		return true;
	}
	//} end from sub server
}
