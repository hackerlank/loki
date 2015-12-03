#pragma once

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "connection.h"

#include <google/protobuf/message.h>

namespace loki
{

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

class protobuf_codec : boost::noncopyable
{
public:
	typedef std::function<void (connection_ptr&, const MessagePtr&)> ProtobufCodecCallback;

	explicit protobuf_codec(ProtobufCodecCallback cb):callback_(cb)
	{
	}
	//protobuf_codec() {}

	static MessagePtr parse(const char* buf, const uint32_t len, uint32_t& error);
	static google::protobuf::Message* createMessage(const std::string& typeName);
	static void send(connection_ptr& conn, const MessagePtr&);
	static void send(connection_ptr& conn, const google::protobuf::Message*);

	static void send(conn_weak_ptr& conn, const MessagePtr&);
	static void send(conn_weak_ptr& conn, const google::protobuf::Message*);

	void handle_message(connection_ptr& conn) const;
private:
	ProtobufCodecCallback callback_;
};
}
