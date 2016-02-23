#include "RecordServer.h"
#include "Callback.h"
#include "ServerType.h"
#include "Record.pb.h"
#include "db.h"

using namespace loki;

RecordServer::RecordServer(io_service_pool& p, const std::string& filename): SubService(p, RECORDSERVER, filename)
{
}

bool RecordServer::Init()
{
	return SubService::Init();
}

void RecordServer::RegisterCallback()
{
	//from sub user
	using namespace Record;
	dispatcher_.registerMsgCallback<Record::t_LoginRecord>(std::bind(onLoginRecord, std::placeholders::_1, std::placeholders::_2));
	/*
	dispatcher_.registerMsgCallback<Record::t_Get_SelectInfo_GateRecord>(std::bind(onSelectInfo, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Record::t_CheckName_GateRecord>(std::bind(onCheckUserName, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Record::t_CreateChar_GateRecord>(std::bind(onCreateCharacter, std::placeholders::_1, std::placeholders::_2));
	*/
}

bool RecordServer::PostInit()
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
	return true;
}
