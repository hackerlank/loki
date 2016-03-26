#include "SessionServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "Record.pb.h"
#include "db.h"

using namespace loki;

SessionServer::SessionServer(io_service_pool& p, const std::string& filename): SubService(p, SESSIONSERVER, filename)
{
}

bool SessionServer::Init()
{
	return SubService::Init();
}

void SessionServer::RegisterCallback()
{
	//from sub user
	using namespace Session;
	dispatcher_.registerMsgCallback<Session::t_LoginSession>(std::bind(onLoginSession, std::placeholders::_1, std::placeholders::_2));
	/*
	dispatcher_.registerMsgCallback<Session::t_Get_SelectInfo_GateSession>(std::bind(onSelectInfo, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Session::t_CheckName_GateSession>(std::bind(onCheckUserName, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Session::t_CreateChar_GateSession>(std::bind(onCreateCharacter, std::placeholders::_1, std::placeholders::_2));
	*/
}

void SessionServer::disconnectRecord(TcpConnPtr conn, const boost::system::error_code& err)
{
	handle_stop();
	LOG(INFO)<<__func__;
}

bool SessionServer::PostInit()
{
	lua_tinker::table common = script_->get<lua_tinker::table>("Common");
	sql::Driver * driver = get_driver_instance();
	if(!driver){
		LOG(ERROR)<<("get driver instance error");
		return false;
	}
	s_dbConn.reset(driver->connect(common.get<const char*>("hostname"), common.get<const char*>("user"), common.get<const char*>("password")));
	if(!s_dbConn){
		LOG(ERROR)<<("connect mysql error");
		return false;
	}
	s_dbConn->setSchema(common.get<const char*>("database"));
	LOG(INFO)<<"Connect Database Success";

	//connect RecordServer
	recordClient.reset(new TcpConnection(pool_.get_io_service()));
	recordClient->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &recordDispatcher, std::placeholders::_1, std::placeholders::_2);
	recordClient->errorHandler = std::bind(&SessionServer::disconnectRecord, this, std::placeholders::_1, std::placeholders::_2);
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
	return true;
}
