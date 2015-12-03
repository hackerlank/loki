/**
 * \file
 * \version  $Id: SuperServer/ServerManager.cpp 0 2015-03-07 15:12:13 xxx $
 * \author  liuqing,liuqing1@ztgame.com
 * \date Sat Mar  7 15:12:13 2015
 * \brief 
 */


#include "ServerManager.h"
#include "protobuf_codec.h"

namespace loki
{

bool ServerManager::add(const context_ptr& ctx)
{
	auto ret = container.insert(std::make_pair(ctx->id(), ctx));
	return (ret.second);
}

void ServerManager::erase(const uint32_t id)
{
	container.erase(id);
}

bool ServerManager::broadcastByID(const uint32_t id, const MessagePtr& msg)
{
	return broadcastByID(id, msg.get());
}

bool ServerManager::broadcastByID(const uint32_t id, const google::protobuf::Message* msg)
{
	auto it = container.find(id);
	if (it == container.end()) return false;
	protobuf_codec::send(it->second->connection(), msg);
	return true;
}

bool ServerManager::broadcastByType(const uint32_t type, const MessagePtr& msg)
{
	return broadcastByType(type, msg.get());
}

bool ServerManager::broadcastByType(const uint32_t type, const google::protobuf::Message* msg)
{
	for (auto it = container.begin();it!=container.end(); ++it)
	{
		if (it->second->type() == type)
		{
			protobuf_codec::send(it->second->connection(), msg);
		}
	}
	return true;
}

bool ServerManager::broadcast(const google::protobuf::Message* msg)
{
	for (auto it = container.begin(); it != container.end(); ++it)
	{
		protobuf_codec::send(it->second->connection(), msg);
	}
	return true;
}

bool ServerManager::broadcast(const MessagePtr& msg)
{
	return broadcast(msg.get());
}

}
