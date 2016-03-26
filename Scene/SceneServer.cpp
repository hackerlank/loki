#include "SceneServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "Scene.pb.h"
#include "Record.pb.h"
#include "Session.pb.h"

using namespace loki;

SceneServer::SceneServer(io_service_pool& p, const std::string& filename): SubService(p, SCENESERVER, filename)
{
}

bool SceneServer::Init()
{
	return SubService::Init();
}

void SceneServer::RegisterCallback()
{
	//from sub user
	using namespace SceneCmd;
	dispatcher_.registerMsgCallback<SceneCmd::t_LoginScene>(std::bind(onLoginScene, std::placeholders::_1, std::placeholders::_2));
	/*
	dispatcher_.registerMsgCallback<Scene::t_Get_SelectInfo_GateScene>(std::bind(onSelectInfo, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Scene::t_CheckName_GateScene>(std::bind(onCheckUserName, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Scene::t_CreateChar_GateScene>(std::bind(onCreateCharacter, std::placeholders::_1, std::placeholders::_2));
	*/
}

void SceneServer::disconnectRecord(TcpConnPtr conn, const boost::system::error_code& err)
{
	handle_stop();
	LOG(INFO)<<__func__;
}

void SceneServer::disconnectSession(TcpConnPtr conn, const boost::system::error_code& err)
{
	handle_stop();
	LOG(INFO)<<__func__;
}

bool SceneServer::PostInit()
{
	//connect RecordServer
	recordClient.reset(new TcpConnection(pool_.get_io_service()));
	recordClient->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &recordDispatcher, std::placeholders::_1, std::placeholders::_2);
	recordClient->errorHandler = std::bind(&SceneServer::disconnectRecord, this, std::placeholders::_1, std::placeholders::_2);
	ServerEntryPtr recordEntry = serverlist_.getEntryByType(RECORDSERVER);
	if (!recordEntry)
	{
		LOG(INFO)<<("can't find record entry");
		return false;
	}
	if (!recordClient->Connect(recordEntry->ip(), recordEntry->port()))
	{
		LOG(INFO)<<("can't connect record entry");
		return false;
	}
	else
	{
		LOG(INFO)<<"Sync Connect Record OK";
		Record::t_LoginRecord send;
		send.set_id(myentry_->id());
		send.set_type(myentry_->type());
		recordClient->SendMessage(&send);
	}

	sessionClient.reset(new TcpConnection(pool_.get_io_service()));
	sessionClient->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &sessionDispatcher, std::placeholders::_1, std::placeholders::_2);
	sessionClient->errorHandler = std::bind(&SceneServer::disconnectSession, this, std::placeholders::_1, std::placeholders::_2);
	ServerEntryPtr sessionEntry = serverlist_.getEntryByType(SESSIONSERVER);
	if (!sessionEntry)
	{
		LOG(INFO)<<("can't find session entry");
		return false;
	}
	if (!sessionClient->Connect(sessionEntry->ip(), sessionEntry->port()))
	{
		LOG(INFO)<<("can't connect session entry");
		return false;
	}
	else
	{
		LOG(INFO)<<"Sync Connect Session OK";
		Session::t_LoginSession send;
		send.set_id(myentry_->id());
		send.set_type(myentry_->type());
		sessionClient->SendMessage(&send);
	}
	return true;
}
