#pragma once

#include "RecordServer.h"

#include "super.pb.h"
#include "record.pb.h"
#include "session.pb.h"
#include "RecordTask.h"
#include "db.h"

namespace Record
{
	bool onLoginRecord(connection_ptr conn, const std::shared_ptr<Record::t_LoginRecord>& msg)
	{
		context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
		if (!context) 
		{
			return false;
		}
		if (context->state() != 0)
		{
			LOG(INFO)<<msg->GetTypeName()<<" should be the first message";
			return false;
		}
		else 
		{
			context->set_id(msg->id());
			context->set_type(msg->type());
			//sub_conns_[msg->id()] = context;
			LOG(INFO)<<__func__<<"ok:"<<conn->get_peer_info()<<",id="<<context->id()<<",type="<<context->type();
			return true;
		}
	}

	bool onSelectInfo(connection_ptr conn, const std::shared_ptr<t_Get_SelectInfo_GateRecord>& msg)
		try
	{
		LOG(INFO)<<__func__<<",accid="<<msg->accid()<<",ip="<<msg->ip();

		std::unique_ptr<sql::Statement> stmt(s_dbConn->createStatement());
		std::ostringstream oss;
		oss<<"SELECT charid,name,type,country FROM `charbase` where accid="<<msg->accid();
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(oss.str()));

		Record::t_Ret_SelectInfo_GateRecord ret;
		ret.set_accid(msg->accid());
		while (res->next())
		{
			auto entry = ret.add_info();
			entry->set_id(res->getUInt("charid"));
			entry->set_name(res->getString("name"));
			entry->set_type(res->getUInt("type"));
			entry->set_type(res->getUInt("country"));
		}
		protobuf_codec::send(conn, &ret);
		return true;
	}
	catch (sql::SQLException &e)
	{
		using namespace std;
		std::ostringstream oss;
		oss << "# ERR: SQLException in " << __FILE__;
		oss << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		oss << "# ERR: " << e.what();
		oss << " (MySQL error code: " << e.getErrorCode();
		oss << ", SQLState: " << e.getSQLState() << " )" << endl;
		LOG(INFO)<<oss.str();
		return false;
	}

	//检查角色名是否已存在
	bool onCheckUserName(connection_ptr conn, const std::shared_ptr<Record::t_CheckName_GateRecord>& msg)
	{
		Record::t_CheckName_Return_GateRecord send;
		send.set_accid(msg->accid());
		send.set_name(msg->name());

		std::unique_ptr<sql::Statement> stmt(s_dbConn->createStatement());
		std::ostringstream oss;
		oss<<"SELECT name FROM `charbase` where name=\""<<msg->name()<<"\"";
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(oss.str()));
		send.set_code(res->rowsCount());
		protobuf_codec::send(conn, &send);
		return true;
	}

	bool onCreateCharacter(connection_ptr conn, const std::shared_ptr<Record::t_CreateChar_GateRecord>& msg)
	{
		std::unique_ptr<sql::Statement> stmt(s_dbConn->createStatement());
		std::ostringstream oss;
		oss<<"insert into `charbase`(accid,name,type,country) values ("<<msg->accid()<<",\""<<msg->name()<<"\","<<msg->type()<<","<<msg->country()<<")";

		Record::t_CreateChar_Return_GateRecord ret;
		ret.set_accid(msg->accid());
		bool result = !stmt->execute(oss.str());
		ret.set_retcode(result?1:0);
		ret.mutable_charinfo()->set_id(0);
		ret.mutable_charinfo()->set_name(msg->name());
		ret.mutable_charinfo()->set_type(msg->type());
		ret.mutable_charinfo()->set_type(msg->country());
		protobuf_codec::send(conn, &ret);
		LOG(INFO)<<__func__<<",create character insert to db, result="<<result?"OK":"Fail";
		return true;
	}
}
