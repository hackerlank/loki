#pragma once

#include "SuperServer.h"
#include "super.pb.h"
#include "login.pb.h"

bool onStartup_Request(TcpConnPtr conn, std::shared_ptr<Super::t_Startup_Request> msg)
{
	LOG(INFO)<<__func__;
	return true;
}

bool onUserVerifyVerCmd(TcpConnPtr conn, std::shared_ptr<::Login::stUserVerifyVerCmd> msg)
{
	LOG(INFO)<<__func__;
	return true;
}
