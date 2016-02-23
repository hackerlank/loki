#pragma once

#include "GateServer.h"
#include "Gate.pb.h"
#include "GateEntity.h"

/*
bool onLoginGate(TcpConnPtr conn, std::shared_ptr<Gate::t_LoginGate> msg) {
	if (conn->GetData() != nullptr) {
		LOG(INFO)<<msg->GetTypeName()<<" should be the first message";
		return false;
	}
	auto context(new GateEntity(conn));
	conn->SetData(context);

	context->set_id(msg->id());
	context->set_type(msg->type());
	//sub_conns_[msg->id()] = context;
	LOG(INFO)<<__func__<<"ok:"<<conn->GetRemoteIP()<<",id="<<context->id()<<",type="<<context->type();
	return true;
}

*/
