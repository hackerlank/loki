#pragma once
#include "zScene.h"
#include "connection.h"
#include "protobuf_codec.h"

class SessionTask;

class SceneSession : public zScene
{
public:
	SessionTask* task_{nullptr};
	std::string filename_;
	uint8_t level_{0};
	SceneSession(SessionTask* task);

	bool sendCmd(loki::MessagePtr msg);
	bool sendCmd(const google::protobuf::Message* msg);

	SessionTask* getTask() { return task_; }

	const std::string filename() const { return filename_; }
};
