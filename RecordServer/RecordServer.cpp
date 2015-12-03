#include "RecordServer.h"
#include "RecordTask.h"
#include <cstdlib>
#include "RecordCallback.h"
#include "db.h"

using namespace loki;

RecordServer::RecordServer(io_service_pool& pool, const std::string& script_file)
	:sub_service(pool, RECORDSERVER, script_file)
{
}

void RecordServer::on_connected(connection_ptr conn)
{
	context_ptr task(new RecordTask(conn));
	conn->set_context(task);
	get_logic_service().post(boost::bind(&RecordServer::add_connection, this, conn));
}

void RecordServer::add_connection(connection_ptr conn)
{
	connections_[conn->get_id()] = conn;
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
}

void RecordServer::remove_connection(connection_ptr conn)
{
	context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
	//sub_conns_.erase(context->id());
	conn->set_context(context_ptr(nullptr));

	connections_.erase(conn->get_id());
	LOG(INFO)<<__func__<<",conn size"<<connections_.size()<<", connid="<<conn->get_id();
}

void RecordServer::on_error(connection_ptr conn)
{
	get_logic_service().post(boost::bind(&RecordServer::remove_connection, this, conn));	
}

void RecordServer::registerCallback()
{
	//from sub user
	using namespace Record;
	dispatcher_.registerMsgCallback<Record::t_LoginRecord>(std::bind(onLoginRecord, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Record::t_Get_SelectInfo_GateRecord>(std::bind(onSelectInfo, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Record::t_CheckName_GateRecord>(std::bind(onCheckUserName, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.registerMsgCallback<Record::t_CreateChar_GateRecord>(std::bind(onCreateCharacter, std::placeholders::_1, std::placeholders::_2));
}

bool RecordServer::start_server()
{
	started_ = true;
	return true;
}

bool RecordServer::init()
{
	if (!sub_service::init())
		return false;

	lua_tinker::table common = script_->get<lua_tinker::table>("Common");

	//{init db
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
	//}
	return true;
}
