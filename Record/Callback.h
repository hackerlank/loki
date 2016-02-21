#pragma once

#include "RecordServer.h"
#include "record.pb.h"
#include "RecordEntity.h"

bool onLoginRecord(TcpConnPtr conn, std::shared_ptr<Record::t_LoginRecord> msg) {
	if (conn->GetData() != nullptr) {
		LOG(INFO)<<msg->GetTypeName()<<" should be the first message";
		return false;
	}
	auto context(new RecordEntity(conn));
	conn->SetData(context);

	context->set_id(msg->id());
	context->set_type(msg->type());
	//sub_conns_[msg->id()] = context;
	LOG(INFO)<<__func__<<"ok:"<<conn->GetRemoteIP()<<",id="<<context->id()<<",type="<<context->type();
	return true;
}

