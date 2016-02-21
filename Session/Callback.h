#pragma once

#include "SessionServer.h"
#include "Session.pb.h"
#include "SessionEntity.h"

bool onLoginSession(TcpConnPtr conn, std::shared_ptr<Session::t_LoginSession> msg) {
	if (conn->GetData() != nullptr) {
		LOG(INFO)<<msg->GetTypeName()<<" should be the first message";
		return false;
	}
	auto context(new SessionEntity(conn));
	conn->SetData(context);

	context->set_id(msg->id());
	context->set_type(msg->type());
	//sub_conns_[msg->id()] = context;
	LOG(INFO)<<__func__<<"ok:"<<conn->GetRemoteIP()<<",id="<<context->id()<<",type="<<context->type();
	return true;
}

