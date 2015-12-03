#pragma once
#include "singleton.h"
#include "connection.h"
#include "ServerList.h"

using loki::connection_ptr;


//降低 SceneClient 和 connection 间的关联

class SceneClientManager : public Singleton<SceneClientManager>
{
public:
	~SceneClientManager();
	void on_connected(connection_ptr conn);
	void on_sync_connected(connection_ptr conn);

	void on_error(connection_ptr conn);
	void on_sync_error(connection_ptr conn);

	//void check_reconnect(const boost::system::error_code& e, connection_ptr conn);

	void init(const std::vector<ServerEntryPtr>& entrys);

public:
	std::map<uint32_t, connection_ptr> sceneclients_;
	//std::map<connection_ptr, SceneClientPtr> sc_;
};
