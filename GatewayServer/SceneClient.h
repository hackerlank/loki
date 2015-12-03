#pragma once

#include "connection.h"
#include <string>
#include "ServerList.h"
#include "context.h"

using namespace loki;

class SceneClient;
typedef std::shared_ptr<SceneClient> SceneClientPtr;

class SceneClientManager;

class SceneClient : public loki::context
{
	friend class SceneClientManager;
public:
	SceneClient(boost::asio::io_service& io, connection_ptr conn, const ServerEntryPtr& e);
	~SceneClient();

	//void init(connection_ptr conn);
	void async_connect(connection_ptr);

	const std::string name() const { return name_; }
	void set_name(const std::string& n) { name_ = n; }
	const std::string ip() const { return  entry_->ip(); }
	uint32_t port() const { return entry_->port(); }
private:
	ServerEntryPtr entry_;
	boost::asio::deadline_timer timer_;

	int connect_times_ = 0;
	std::string name_;
};
