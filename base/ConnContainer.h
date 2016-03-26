/**
 * \file
 * \version  $Id: ServerManager.h 0 2015-03-07 14:41:57 xxx $
 * \author  liuqing,liuqing1@ztgame.com
 * \date Sat Mar  7 14:41:57 2015
 * \brief task container
 */

#pragma once
#include <unordered_map>
#include "context.h"
#include "TcpConn.h"

namespace loki
{

class ConnContainer
{
public:
	bool Add(const TcpConnPtr& ctx)
	{
		auto ret = container.insert(std::make_pair(ctx->GetID(), ctx));
		return (ret.second);
	}
	void RemoveByID(const uint32_t id)
	{
		container.erase(id);
	}
	bool broadcastByID(const uint32_t id, const MessagePtr& msg)
	{
		return broadcastByID(id, msg.get());
	}
	bool broadcastByID(const uint32_t id, const google::protobuf::Message* msg)
	{
		auto it = container.find(id);
		if (it == container.end()) return false;
		it->second->SendMessage(msg);
		return true;
	}
	bool broadcastByType(const uint32_t type, const MessagePtr& msg)
	{
		return broadcastByType(type, msg.get());
	}

	bool broadcastByType(const uint32_t type, const google::protobuf::Message* msg)
	{
		for (auto it = container.begin();it!=container.end(); ++it)
		{
			if (it->second->GetType() == type)
			{
				it->second->SendMessage(msg);
			}
		}
		return true;
	}

	bool broadcast(const MessagePtr& msg)
	{
		return broadcast(msg.get());
	}
	bool broadcast(const google::protobuf::Message* msg)
	{
		for (auto it = container.begin(); it != container.end(); ++it)
		{
			it->second->SendMessage(msg);
		}
		return true;
	}

private:
	std::unordered_map<uint32_t, TcpConnPtr> container;
};

}
