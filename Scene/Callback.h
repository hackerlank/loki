#pragma once

#include "SceneServer.h"
#include "Scene.pb.h"
#include "SceneEntity.h"

bool onLoginScene(TcpConnPtr conn, std::shared_ptr<SceneCmd::t_LoginScene> msg) {
	if (conn->GetData() != nullptr) {
		LOG(INFO)<<msg->GetTypeName()<<" should be the first message";
		return false;
	}
	auto context(new SceneEntity(conn));
	conn->SetData(context);

	context->set_id(msg->id());
	context->set_type(msg->type());
	//sub_conns_[msg->id()] = context;
	LOG(INFO)<<__func__<<"ok:"<<conn->GetRemoteIP()<<",id="<<context->id()<<",type="<<context->type();
	return true;
}

