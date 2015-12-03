#include "SceneSession.h"
#include "SessionTask.h"

using namespace loki;

SceneSession::SceneSession(SessionTask* task):
	zScene(),task_(task)
{
}

bool SceneSession::sendCmd(loki::MessagePtr msg)
{
	return true;
}

bool SceneSession::sendCmd(const google::protobuf::Message* msg)
{
	return true;
}
