#include "GateServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "Record.pb.h"
#include "Session.pb.h"
#include "Scene.pb.h"
#include "db.h"

using namespace loki;

GateServer::GateServer(io_service_pool& p, const std::string& filename): SubService(p, GATEWAYSERVER, filename)
{
}

bool GateServer::Init()
{
	return SubService::Init();
}

void GateServer::RegisterCallback()
{
	//from sub user
	using namespace Gate;
	//dispatcher_.registerMsgCallback<Gate::t_LoginGate>(std::bind(onLoginGate, std::placeholders::_1, std::placeholders::_2));
	/*
	dispatcher_.registerMsgCallback<Gate::t_Get_SelectInfo_GateGate>(std::bind(onSelectInfo, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Gate::t_CheckName_GateGate>(std::bind(onCheckUserName, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Gate::t_CreateChar_GateGate>(std::bind(onCreateCharacter, std::placeholders::_1, std::placeholders::_2));
	*/
}

void GateServer::disconnectRecord(TcpConnPtr conn, const boost::system::error_code& err)
{
	handle_stop();
	LOG(INFO)<<__func__;
}

void GateServer::disconnectSession(TcpConnPtr conn, const boost::system::error_code& err)
{
	handle_stop();
	LOG(INFO)<<__func__;
}

bool GateServer::PostInit()
{
	//connect RecordServer
	recordClient.reset(new TcpConnection(pool_.get_io_service()));
	recordClient->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &recordDispatcher, std::placeholders::_1, std::placeholders::_2);
	recordClient->errorHandler = std::bind(&GateServer::disconnectRecord, this, std::placeholders::_1, std::placeholders::_2);
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

	//connect SessionServer
	sessionClient.reset(new TcpConnection(pool_.get_io_service()));
	sessionClient->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &sessionDispatcher, std::placeholders::_1, std::placeholders::_2);
	sessionClient->errorHandler = std::bind(&GateServer::disconnectSession, this, std::placeholders::_1, std::placeholders::_2);
	ServerEntryPtr sessionEntry = serverlist_.getEntryByType(SESSIONSERVER);
	if (!sessionEntry)
	{
		LOG(INFO)<<("can't find Session entry");
		return false;
	}
	if (!sessionClient->Connect(sessionEntry->ip(), sessionEntry->port()))
	{
		LOG(INFO)<<("can't connect Session entry");
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

	std::vector<ServerEntryPtr> scenesentry = serverlist_.getEntrysByType(SCENESERVER);
	if (scenesentry.empty())
	{
		LOG(INFO)<<("can't find scene entry");
		return false;
	}
	else
	{
		LOG(INFO)<<"scene entry num="<<scenesentry.size();
	}
	for (size_t i = 0;i<scenesentry.size(); ++i)
	{
		TcpConnPtr conn(new TcpConnection(pool_.get_io_service()));
		auto data = new GateEntity(conn);
		data->entryPtr = scenesentry[i];
		conn->SetData(data);
		conn->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &sceneDispatcher, std::placeholders::_1, std::placeholders::_2);
		conn->errorHandler = std::bind(&GateServer::disconnectScene, this, std::placeholders::_1, std::placeholders::_2);
		conn->connectedHandler = std::bind(&GateServer::handleConnectScene, this, std::placeholders::_1, std::placeholders::_2);
		conn->AsyncConnect(scenesentry[i]->ip(), scenesentry[i]->port());
	}
	return true;
}

void GateServer::disconnectScene(TcpConnPtr conn, const boost::system::error_code& err)
{
}

void GateServer::handleConnectScene(TcpConnPtr conn, const boost::system::error_code& err)
{
	if (!err)
	{
		GateEntity* data = (GateEntity*)(conn->GetData());
		SceneCmd::t_LoginScene send;
		send.set_id(data->entryPtr->id());
		send.set_type(data->entryPtr->type());
		conn->SendMessage(&send);
		LOG(INFO)<<"Connected Scene";
		sceneClients[data->entryPtr->id()] = conn;
	}
}
