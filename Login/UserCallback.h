#pragma once

#include "LoginServer.h"
#include "Login.pb.h"
#include "UserEntity.h"
#include "GYListManager.h"
#include "login_return.h"
#include "gamezone.h"

namespace Login
{
	bool OnUserVerifyVerCmd(TcpConnPtr conn, std::shared_ptr<Login::stUserVerifyVerCmd> msg)
	{
		if (conn->GetData() != nullptr) {
			LOG(INFO)<<msg->GetTypeName()<<", should be the first message";
			return false;
		}
		LOG(INFO)<<"Get Client Version = "<<msg->version();
		//check version
		if (msg->version() < 0)
		{
			LOG(INFO)<<msg->GetTypeName()<<", version error";
			return false;
		}
		UserEntity* context(new UserEntity(conn));
		conn->SetData(context);
		return true;
	}

	void loginResult(TcpConnPtr conn, const uint32_t code)
	{
		stServerReturnLoginFailedCmd cmd;
		cmd.set_code(code);
		conn->SendMessage(&cmd);
		LOG(INFO)<<"loginResult code="<<code;
	}

	bool OnUserRequestLogin(TcpConnPtr conn, std::shared_ptr<Login::stUserRequestLoginCmd>& msg)
		try
	{
		/*
		if (conn->GetData() == nullptr) {
			LOG(INFO)<<"message sequence is error";
			return false;
		}
		*/
		if (msg->account().size() > 32 || msg->passwd().size() > 32) 
		{
			loginResult(conn, LOGIN_RETURN_PASSWORDERROR);
			return true;
		}
		std::unique_ptr<sql::Statement> stmt(s_dbConn->createStatement());
		std::ostringstream os;
		os<<"SELECT account,password,isforbid,accid FROM `LOGIN` where account=\""<<msg->account()<<"\"";
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(os.str()));
		if (res->rowsCount() != 1)
		{
			loginResult(conn, LOGIN_RETURN_PASSWORDERROR);
			return true;
		}
		if (!res->next())
			return false;
		if (res->getString("ACCOUNT") != msg->account() || res->getString("PASSWORD") != msg->passwd())
		{
			loginResult(conn, LOGIN_RETURN_PASSWORDERROR);
			return true;
		}
		if (res->getBoolean("isforbid"))
		{
			loginResult(conn, LOGIN_RETURN_IDINCLOSE);
			return true;
		}
		LOG(INFO)<<"Login account ok "<<msg->account();
		GameZone_t zone;
		zone.game = msg->game();
		zone.zone = msg->zone();
		//查找指定区的网关
		t_GYList_FL_ptr gate = GYListManager::instance().getAvl(zone);
		if (!gate)
		{
			loginResult(conn, LOGIN_RETURN_GATEWAYNOTAVAILABLE);
			return true;
		}
		if (gate->num_online() >= 10000)
		{
			loginResult(conn, LOGIN_RETURN_USERMAX);
			return true;
		}
		//LoginServer::instance().add_user_connection(conn);
		t_NewSession_Session tCmd;
		tCmd.mutable_session()->set_accid(res->getUInt("accid"));
		tCmd.mutable_session()->set_gatewayid(gate->serverid());
		tCmd.mutable_session()->set_ip(gate->ip());
		tCmd.mutable_session()->set_port(gate->port());
		tCmd.mutable_session()->set_logintempid(conn->GetID());
		tCmd.mutable_session()->set_key(boost::lexical_cast<std::string>(rand()));

		LoginServer::instance().userConns.Add(conn->GetID(), conn);
		if (!LoginServer::instance().serverConns.broadcastByID(zone.id, &tCmd))
		{
			LOG(ERROR)<<"Send To No one";
		}
		LOG(INFO)<<"Login verify ok, send info to game server to continue verify";
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
};

