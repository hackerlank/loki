#include "context.h"

namespace loki
{

context::~context()
{
}

void context::sendCmd(const google::protobuf::Message* msg)
{
	if (auto conn = conn_.lock())
	{
		protobuf_codec::send(conn, msg);
	}
}

void context::sendCmd(const MessagePtr& msg)
{
	sendCmd(msg.get());
}

connection_ptr context::lock()
{
	auto conn = conn_.lock();
	return conn;
}

}

