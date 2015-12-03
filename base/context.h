#pragma once
#include <boost/noncopyable.hpp>
#include <memory>	// for std::shared_ptr
#include "connection.h"
#include "protobuf_codec.h"

namespace loki
{

//interface class
class context;
typedef std::shared_ptr<context> context_ptr;

class context : boost::noncopyable
{
public:
	context(const connection_ptr& conn): conn_(conn) {}
	virtual ~context();

	void set_id(const uint32_t id) { id_ = id; }
	void set_type(const uint32_t type) { type_ = type; }

	const int state() const { return state_; }
	const uint32_t id() const { return id_; }
	const uint32_t type() const { return type_; }


	const conn_weak_ptr& connection() const { return conn_; }
	conn_weak_ptr& connection() { return conn_; }	//avoid cycle reference

	connection_ptr lock();

	void sendCmd(const google::protobuf::Message* msg);
	void sendCmd(const MessagePtr& msg);
protected:
	conn_weak_ptr conn_;
	uint32_t id_ = 0;
	uint32_t type_ = 0;
	int state_ = 0;
};

template<class T>
std::shared_ptr<T> conn_cast(connection_ptr& conn)
{
	context_ptr context(boost::any_cast<context_ptr>(conn->get_context()));
	std::shared_ptr<T> task = std::dynamic_pointer_cast<T>(context);
	return task;
}

template<class T>
std::shared_ptr<T> conn_release_cast(connection_ptr& conn)
{
	context_ptr context(boost::any_cast<context_ptr>(conn->release_context()));
	std::shared_ptr<T> task = std::dynamic_pointer_cast<T>(context);
	return task;
}

}
