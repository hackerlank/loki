/**
 * \file
 * \version  $Id: ServerManager.h 0 2015-03-07 14:41:57 xxx $
 * \author  liuqing,liuqing1@ztgame.com
 * \date Sat Mar  7 14:41:57 2015
 * \brief task container
 */

#pragma once
#include "singleton.h"
#include <unordered_map>
#include "context.h"
#include "protobuf_dispatcher.h"

namespace loki
{

class ServerManager: public Singleton<ServerManager>
{
public:
	ServerManager() {}
	bool add(const context_ptr& ctx);
	void erase(const uint32_t id);
	bool broadcastByID(const uint32_t id, const MessagePtr& msg);
	bool broadcastByID(const uint32_t id, const google::protobuf::Message* msg);
	bool broadcastByType(const uint32_t type, const MessagePtr& msg);
	bool broadcastByType(const uint32_t type, const google::protobuf::Message* msg);

	bool broadcast(const MessagePtr& msg);
	bool broadcast(const google::protobuf::Message* msg);

private:
	std::unordered_map<uint32_t, context_ptr> container;
};

}
