#include "SceneClientManager.h"
#include "GatewayServer.h"
#include "SceneClient.h"
#include "scene.pb.h"
#include "protobuf_codec.h"
#include "compressor_zlib.h"
#include "encryptor_aes.h"
#include <boost/lexical_cast.hpp>

using namespace loki;

SceneClientManager::~SceneClientManager()
{
	LOG(INFO)<<__func__;
}

void SceneClientManager::on_connected(connection_ptr conn)
{
	GatewayServer::getSingleton().get_logic_service().post(boost::bind(&SceneClientManager::on_sync_connected, this, conn));
}

void SceneClientManager::on_sync_connected(connection_ptr conn)
{
	SceneClientPtr context = loki::conn_cast<SceneClient>(conn);
	if (!context) return ;
	LOG(INFO)<<__func__<<",id="<<context->id()<<",name="<<context->name()<<",type="<<context->type()<<",ip="<<context->ip()<<",port="<<context->port(); 
	//add to container                             
	sceneclients_[context->id()] = conn;

	SceneCmd::t_LoginScene send; 
	send.set_id(context->id());
	send.set_type(context->type());                      
	//send.set_name(std::string('a', 36));              
	protobuf_codec::send(conn, &send);                                                            
}

void SceneClientManager::on_error(connection_ptr conn)
{
	GatewayServer::getSingleton().get_logic_service().post(boost::bind(&SceneClientManager::on_sync_error, this, conn));
}

void SceneClientManager::on_sync_error(connection_ptr conn)
{
	SceneClientPtr context = loki::conn_cast<SceneClient>(conn);
	if (!context) return ;
	LOG(INFO)<<__func__<<",id="<<context->id()<<",name="<<context->name()<<",type="<<context->type()<<",ip="<<context->ip()<<",port="<<context->port(); 
	if (conn->is_connected())
		sceneclients_.erase(context->id());
	context->timer_.expires_from_now(boost::posix_time::seconds(5));
	context->timer_.async_wait(boost::bind(&connection::async_connect, conn, context->ip(), context->port()));
}

/*
void SceneClientManager::check_reconnect(const boost::system::error_code& e, connection_ptr conn)
{
	if (e != boost::asio::error::operation_aborted)
	{
		SceneClientPtr context = loki::conn_cast<SceneClient>(conn);
		auto conn = context->lock();
		if (conn && !conn->is_connected())
		{
			context->async_connect(conn);
		}
		context->timer_.expires_from_now(boost::posix_time::seconds(5));
		context->timer_.async_wait(boost::bind(&SceneClientManager::check_reconnect, SceneClientManager::instance_ptr(), _1, conn));
	}
	else
	{
		LOG(INFO)<<__func__<<",operation cancelled";
	}

}
*/

void SceneClientManager::init(const std::vector<ServerEntryPtr>& scenesentry)
{
	if (scenesentry.empty())
	{
		LOG(INFO)<<("can't find scene entry");
		return ;
	}
	else
	{
		LOG(INFO)<<"scene entry num="<<scenesentry.size();
	}

	for (size_t i=0;i<scenesentry.size(); ++i)
	{
		ServerEntryPtr e = scenesentry[i];
		connection_ptr conn(new connection(GatewayServer::instance().get_io_service(), std::string("sceneclient") + boost::lexical_cast<std::string>(i)));
		context_ptr client(new SceneClient(conn->get_io_service(), conn, e));
		conn->set_context(client);

		conn->set_compressor(std::shared_ptr<loki::compressor>(new loki::compressor_zlib()));

		char key[32]= {'a', 'b', 'c'};
		std::shared_ptr<loki::encryptor_aes> aes(new loki::encryptor_aes());
		aes->setkey((const unsigned char*)key,256);
		conn->set_encryptor(aes);
		/////////////////////

		conn->set_connected_handler(std::bind(&SceneClientManager::on_connected, this, std::placeholders::_1));
		conn->set_error_handler(std::bind(&SceneClientManager::on_error, this, std::placeholders::_1));

		//直接放入主逻辑线程处理
		conn->set_msg_handler(std::bind(&GatewayServer::on_msg, GatewayServer::getSingletonPtr(), std::placeholders::_1));

		conn->async_connect(e->ip(), e->port());

		//start timer
		//timer_.expires_from_now(boost::posix_time::seconds(5));
		//timer_.async_wait(boost::bind(&SceneClientManager::check_reconnect, SceneClientManager::instance_ptr(), _1, conn));
	}

}
