#pragma once

#include "LoginServer.h"
#include "Login.pb.h"
#include "GYListManager.h"
#include "login_return.h"
#include "db.h"
#include "ServerManager.h"
#include <sstream>

namespace Login
{
	//{ from user
	//the very first message from player, send version
	bool verifyUserConn(connection_ptr conn, const std::shared_ptr<Login::stUserVerifyVerCmd>& msg)
	{
		LoginTaskPtr context(boost::any_cast<LoginTaskPtr>(conn->get_context()));
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
			context->set_version(msg->version());
			LOG(INFO)<<__func__<<", user login:"<<conn->get_peer_ip()<<","<<conn->get_peer_port()<<",verison="<<msg->version();
			return true;
		}
	}
	void loginResult(connection_ptr conn, const uint32_t code)
	{
		stServerReturnLoginFailedCmd cmd;
		cmd.set_code(code);
		protobuf_codec::send(conn, &cmd);
		LOG(INFO)<<"loginResult code="<<code;
	}
	bool userRequestLogin(connection_ptr conn, const std::shared_ptr<Login::stUserRequestLoginCmd>& msg)
		try
	{
		if (msg->account().size() > 32 || msg->passwd().size() > 32) 
		{
			loginResult(conn, LOGIN_RETURN_PASSWORDERROR);
			return false;
		}
		std::unique_ptr<sql::Statement> stmt(s_dbConn->createStatement());
		std::ostringstream os;
		os<<"SELECT account,password,isforbid,accid FROM `LOGIN` where account=\""<<msg->account()<<"\"";
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(os.str()));
		if (res->rowsCount() != 1)
		{
			loginResult(conn, LOGIN_RETURN_PASSWORDERROR);
			return false;
		}
		if (!res->next())
			return false;
		if (res->getString("ACCOUNT") != msg->account() || res->getString("PASSWORD") != msg->passwd())
		{
			loginResult(conn, LOGIN_RETURN_PASSWORDERROR);
			return false;
		}
		if (res->getBoolean("isforbid"))
		{
			loginResult(conn, LOGIN_RETURN_IDINCLOSE);
			return false;
		}
		GameZone_t zone;
		zone.game = msg->game();
		zone.zone = msg->zone();
		//查找指定区的网关
		t_GYList_FL_ptr gate = GYListManager::instance().getAvl(zone);
		if (!gate)
		{
			loginResult(conn, LOGIN_RETURN_GATEWAYNOTAVAILABLE);
			return false;
		}
		if (gate->num_online() >= 10000)
		{
			loginResult(conn, LOGIN_RETURN_USERMAX);
			return false;
		}
		LoginServer::instance().add_user_connection(conn);
		t_NewSession_Session tCmd;
		tCmd.mutable_session()->set_accid(res->getUInt("accid"));
		tCmd.mutable_session()->set_gatewayid(gate->serverid());
		tCmd.mutable_session()->set_ip(gate->ip());
		tCmd.mutable_session()->set_port(gate->port());
		tCmd.mutable_session()->set_logintempid(conn->get_id());
		//发送到SuperServer连接
		ServerManager::instance().broadcastByID(zone.id, &tCmd);
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

	//} end from user
}
