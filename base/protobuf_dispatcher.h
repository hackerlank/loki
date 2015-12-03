#pragma once

#include <boost/noncopyable.hpp>
#include <functional>	//for std::bind
#include <memory>		//for shared_ptr

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <unordered_map>
#include <google/protobuf/message.h>
#include "logger.h"

namespace loki
{

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

class callback : boost::noncopyable
{
public:
	virtual ~callback() {}
	virtual bool operator()(connection_ptr& conn, const MessagePtr& msg) = 0;
};

template<class T>
class callbackT : public callback
{
public:
	typedef boost::function<bool (connection_ptr&, const std::shared_ptr<T>& )> ProtobufMessageTCallback;

	callbackT(const ProtobufMessageTCallback& cb):callback_(cb)
	{}

	virtual bool operator()(connection_ptr& conn, const MessagePtr& msg)
	{
		std::shared_ptr<T> concrete = std::dynamic_pointer_cast<T>(msg);
		assert(concrete != nullptr);
		return callback_(conn, concrete);
	}

private:
	ProtobufMessageTCallback callback_;
};

class protobuf_dispatcher
{
public:
	typedef boost::function<bool (connection_ptr&, const MessagePtr&)> ProtobufMessageCallback;

	explicit protobuf_dispatcher(const ProtobufMessageCallback& cb):defaultCb_(cb)
	{}

	protobuf_dispatcher(){}

	void onProtobufMessage(connection_ptr& conn, const MessagePtr& msg)
	{
		auto it = callbacks_.find(msg->GetDescriptor());
		if (it != callbacks_.end())
		{
			if (!(*(it->second))(conn, msg))
			{
				conn->close();
				LOG(INFO)<<"close connid="<<conn->get_id()<<", last msg name="<<msg->GetTypeName();
			}
		}
		else
		{
			/*
			if (!defaultCb_(conn, msg))
			{
				conn->close();
				LOG(INFO)<<"close connid="<<conn->get_id()<<", last msg name="<<msg->GetTypeName();
			}
			*/
			LOG(INFO)<<"Unknown message "<<msg->GetTypeName();
		}
	}

	template<class T>
	void registerMsgCallback(const typename callbackT<T>::ProtobufMessageTCallback& cb)
	{
		std::shared_ptr<callbackT<T> > pb(new callbackT<T>(cb));
		callbacks_[T::descriptor()] = pb;
	}
	void clear() { callbacks_.clear(); }

private:
	typedef std::unordered_map<const google::protobuf::Descriptor*, std::shared_ptr<callback> > CallbackMap;

	CallbackMap callbacks_;
	ProtobufMessageCallback defaultCb_;
};
}
