#include "SceneClient.h"
#include "GatewayServer.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "scene.pb.h"
#include <boost/lexical_cast.hpp>
#include "compressor_zlib.h"
#include "encryptor_aes.h"
#include "SceneClientManager.h"

SceneClient::SceneClient(boost::asio::io_service& io, connection_ptr conn, const ServerEntryPtr& e):
	context(conn),
	entry_(e), timer_(io)
{
	set_id(e->id());
	set_type(e->type());
	name_ = e->name();
}

/*
void SceneClient::init(connection_ptr conn)
{
	conn->set_reuse(true);
	conn->set_compressor(std::shared_ptr<loki::compressor>(new loki::compressor_zlib()));

	char key[32]= {'a', 'b', 'c'};
	std::shared_ptr<loki::encryptor_aes> aes(new loki::encryptor_aes());
	aes->setkey((const unsigned char*)key,256);
	conn->set_encryptor(aes);
	/////////////////////

	conn->set_connected_handler(std::bind(&SceneClientManager::on_connected, SceneClientManager::instance_ptr(), std::placeholders::_1));
	conn->set_error_handler(std::bind(&SceneClientManager::on_error, SceneClientManager::instance_ptr(), std::placeholders::_1));

	//直接放入主逻辑线程处理
	conn->set_msg_handler(std::bind(&GatewayServer::on_msg, GatewayServer::getSingletonPtr(), std::placeholders::_1));

	//start timer
	timer_.expires_from_now(boost::posix_time::seconds(5));
	timer_.async_wait(boost::bind(&SceneClientManager::check_reconnect, SceneClientManager::instance_ptr(), _1, conn));
}
*/

void SceneClient::async_connect(connection_ptr conn)
{
	conn->async_connect(entry_->ip(), static_cast<uint16_t>(entry_->port()));
	LOG(INFO)<<__func__<<",id="<<entry_->id()<<",name="<<entry_->name()<<",type="<<entry_->type()<<",ip="<<entry_->ip()<<",port="<<entry_->port()<<",time="<<connect_times_++;
}

SceneClient::~SceneClient()
{
	LOG(INFO)<<__func__<<",id="<<entry_->id()<<",name="<<entry_->name()<<",type="<<entry_->type()<<",ip="<<entry_->ip()<<",port="<<entry_->port();
}
