#pragma once
#include <boost/noncopyable.hpp>
#include <memory>	// for std::shared_ptr
#include "TcpConn.h"

namespace loki
{

//interface class
class ConnEntity;
typedef std::shared_ptr<ConnEntity> ConnEntityPtr;

class ConnEntity : boost::noncopyable
{
public:
	ConnEntity(const TcpConnPtr conn): conn_(conn) {}
	virtual ~ConnEntity(){}

	void set_id(const uint32_t id) { id_ = id; }
	void set_type(const uint32_t type) { type_ = type; }

	const int state() const { return state_; }
	const uint32_t id() const { return id_; }
	const uint32_t type() const { return type_; }


	TcpConnPtr Connection() { return conn_; }

	void SendCmd(const google::protobuf::Message* msg) { conn_->SendMessage(msg); }
	void SendCmd(const MessagePtr msg) { conn_->SendMessage(msg); }
protected:
	TcpConnPtr conn_;
	uint32_t id_ = 0;
	uint32_t type_ = 0;
	int state_ = 0;
};

}
