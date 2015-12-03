#pragma once

#include "SceneServer.h"

#include "super.pb.h"
#include "record.pb.h"
#include "session.pb.h"
#include "scene.pb.h"
#include "SceneManager.h"
#include "ServerManager.h"

namespace tina
{
	bool onLoginScene(connection_ptr conn, const std::shared_ptr<SceneCmd::t_LoginScene>& msg)
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
			LOG(INFO)<<__func__<<", ok:"<<conn->get_peer_info()<<",id="<<msg->id()<<",type="<<msg->type();
			ServerManager::instance().add(context);
			return true;
		}
	}

	bool onRegScene_ret_SceneSession(connection_ptr conn, const std::shared_ptr<Session::t_regScene_ret_SceneSession>& msg)
	{
		Scene* scene = (Scene*)SceneManager::instance().getSceneByTempID(msg->tempid());
		if (scene)
		{
			if (msg->value() == 1)
			{
				LOG(INFO)<<"register map success: name="<<scene->name<<",id="<<scene->id;

				//broadcast to gate
				SceneCmd::t_fresh_MapIndex map;
				map.set_size(1);
				auto m = map.add_mps();
				m->set_maptempid(scene->tempid);
				m->set_mapx(scene->getScreenX());
				m->set_mapy(scene->getScreenY());
				ServerManager::instance().broadcast(&map);
			}
			else
			{
			}
			return true;
		}
		return true;
	}

}
