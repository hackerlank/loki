#pragma once

#include <boost/function.hpp>
#include <google/protobuf/message.h>
#include <boost/noncopyable.hpp>
#include <functional>	//for std::bind
#include <memory>		//for shared_ptr
#include "TcpConn.h"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <unordered_map>
#include <google/protobuf/message.h>
#include "logger.h"

namespace loki
{

	typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

	class Proto 
	{
		public:
			//typedef std::function<void (TcpConnPtr&, const MessagePtr&)> ProtobufCodecCallback;

			/*explicit Proto(ProtobufCodecCallback cb):callback_(cb)
			  {
			  }
			  */

			//static MessagePtr parse(const char* buf, const uint32_t len, uint32_t& error);
			static google::protobuf::Message* CreateMessage(const std::string& typeName)
			{
				google::protobuf::Message* message = NULL;
				const google::protobuf::Descriptor* descriptor =
					google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
				if (descriptor)
				{
					const google::protobuf::Message* prototype =
						google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
					if (prototype)
					{
						message = prototype->New();
					}
				} 
				return message;
			}
			//static void send(TcpConnPtr& conn, const MessagePtr&);
			//static void send(TcpConnPtr& conn, const google::protobuf::Message*);

			//static void send(conn_weak_ptr& conn, const MessagePtr&);
			//static void send(conn_weak_ptr& conn, const google::protobuf::Message*);

			//void handle_message(TcpConnPtr& conn) const;
			//
			/*private:
			  ProtobufCodecCallback callback_;
			  */
	};

	//typedef tcp_connection::pointer TcpConnPtr;

	class callback : boost::noncopyable
	{
		public:
			virtual ~callback() {}
			virtual bool operator()(TcpConnPtr conn, const MessagePtr msg) = 0;
	};

	template<class T>
		class callbackT : public callback
	{
		public:
			typedef boost::function<bool (TcpConnPtr, const std::shared_ptr<T> )> ProtobufMessageTCallback;

			callbackT(const ProtobufMessageTCallback& cb):callback_(cb)
		{}

			virtual bool operator()(TcpConnPtr conn, const MessagePtr msg)
			{
				std::shared_ptr<T> concrete = std::dynamic_pointer_cast<T>(msg);
				assert(concrete != nullptr);
				return callback_(conn, concrete);
			}

		private:
			ProtobufMessageTCallback callback_;
	};

	class ProtoDispatcher
	{
		public:
			typedef boost::function<bool (TcpConnPtr, const MessagePtr)> ProtobufMessageCallback;

			explicit ProtoDispatcher(const ProtobufMessageCallback& cb):defaultCb_(cb)
		{}

			ProtoDispatcher(){}

			void onProtobufMessage(TcpConnPtr conn, const MessagePtr msg)
			{
				auto it = callbacks_.find(msg->GetDescriptor());
				if (it != callbacks_.end())
				{
					if (!(*(it->second))(conn, msg))
					{
						conn->close();
						LOG(INFO)<<"close connid="<<conn->GetID()<<", last msg name="<<msg->GetTypeName();
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
