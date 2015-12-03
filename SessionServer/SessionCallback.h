#pragma once

#include "SessionServer.h"
#include "SceneSessionManager.h"

namespace Session
{

	bool onLogin(connection_ptr conn, const std::shared_ptr<Session::t_LoginSession>& msg)
	{
		context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
		if (!context) 
		{
			return false;
		}
		if (context->state() != 0)
		{
			return false;
		}
		else 
		{
			context->set_id(msg->id());
			context->set_type(msg->type());
			//sub_conns_[context->id()] = context;
			LOG(INFO)<<__func__<<", ok:"<<conn->get_peer_info()<<",id="<<msg->id()<<",type="<<msg->type();
			return true;
		}
	}

	bool onRegUser_GateSession(connection_ptr conn, const std::shared_ptr<Session::t_regUser_GateSession>& msg)
	{
		return true;
	}

	bool onRegScene_SceneSession(connection_ptr conn, const std::shared_ptr<Session::t_regScene_SceneSession>& msg)
	{
		auto task = loki::conn_cast<SessionTask>(conn);
		if (!task) return true;
		SceneSession* scene = createSceneSession(task.get());
		if (scene)
		{
			scene->id = msg->id();
			scene->tempid = msg->tempid();
			strncpy(scene->name, msg->name().c_str(), sizeof(scene->name));
			scene->filename_ = msg->filename();
			Session::t_regScene_ret_SceneSession send;
			send.set_tempid(msg->tempid());
			if (SceneSessionManager::instance().addScene(scene))
			{
				send.set_value(1);
			}
			else
			{
				send.set_value(0);
			}
			LOG(INFO)<<"register map "<<(send.value() == 1?"ok":"failed")<<": name="<<scene->name<<",id="<<scene->id<<",tempid="<<scene->tempid<<",filename="<<scene->filename();
			protobuf_codec::send(conn, &send);
		}
		return true;
	}

}
