#include "protobuf_codec.h"
#include <utility>
#include <zlib.h>
#include "logger.h"

namespace loki
{

google::protobuf::Message* protobuf_codec::createMessage(const std::string& typeName)
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


MessagePtr protobuf_codec::parse(const char* buf, const uint32_t len, uint32_t& error)
{
	MessagePtr message;

	do
	{
		//msg_name_len include the end character '\0'
		const uint16_t msg_name_len = *(const uint16_t*)&buf[0];		//TODO: ntoh()

		if (msg_name_len > 128 || msg_name_len < MIN_MSG_NAME_SIZE || msg_name_len + MSG_NAME_LENGTH_SIZE > len)
		{
			LOG(ERROR)<<"message name length error length = "<<msg_name_len<<", total="<<len;
			error = 1;
			break;
		}

		char msgname[128] = {'\0'};
		strncpy(msgname, (const char*)(buf + MSG_NAME_LENGTH_SIZE), msg_name_len);
		const uint16_t n = strlen(msgname) + 1;
		if (n != msg_name_len)
		{
			LOG(ERROR)<<"message name length error (received="<<n<<", expected="<<msg_name_len<<", msgname="<<msgname<<")";
			error = 2;
			break;
		}
		LOG(INFO)<<"[protobuf_codec],parse, msg_name_len="<<msg_name_len<<", msgname="<<msgname;

		message.reset(createMessage(msgname));
		if (!message)
		{
			LOG(ERROR)<<"unrecognized message name = "<<msgname;
			error = 3;
			break;
		}

		if (!message->ParseFromString(std::string(buf + MSG_NAME_LENGTH_SIZE + msg_name_len, len - MSG_NAME_LENGTH_SIZE - msg_name_len)))
		{
			LOG(ERROR)<<"Parse error msgname="<<msgname;
			error = 4;
			break;
		}
	}while(false);

	return message;
}

void protobuf_codec::handle_message(connection_ptr& conn) const
{
	loki::buffer* buffer = conn->get_buffer();
	while (buffer->data_size() >= MSG_NAME_LENGTH_SIZE + MIN_MSG_NAME_SIZE + MSG_HEADER_LENGTH)	
	{
		const uint32_t flag = buffer->get<uint32_t>();
		const uint32_t msg_len = flag & 0x0000ffff;
		if (msg_len + MSG_HEADER_LENGTH > buffer->data_size())
		{
			//message not enough, wait
			break;
		}
		if (msg_len > MAX_MSG_LENGTH || msg_len < MSG_NAME_LENGTH_SIZE + MIN_MSG_NAME_SIZE)
		{
			LOG(ERROR)<<"message length error "<<msg_len<<",readable="<<buffer->data_size();
			conn->close();
			break;
		}

		char buff[MAX_MSG_LENGTH];
		size_t len = sizeof(buff);
		LOG(INFO)<<__func__<<", flag="<<flag;
		bool ret = conn->restore_msg((const char*)buffer->data_ptr() + MSG_HEADER_LENGTH, flag, buff, len);
		if (!ret)
		{
			LOG(ERROR)<<"restore msg error";
			conn->close();
			break;
		}

		uint32_t errorcode = 0;
		MessagePtr message = parse(buff, len, errorcode);
		if (errorcode == 0 && message)
		{
			callback_(conn, message);
			buffer->consume(MSG_HEADER_LENGTH + msg_len);
		}
		else
		{
			LOG(ERROR)<<"Parse message error ,errorcode="<<errorcode;
			conn->close();
			break;
		}
	}
}

void protobuf_codec::send(connection_ptr& conn, const MessagePtr& msg)
{
	send(conn, msg.get());
}

void protobuf_codec::send(connection_ptr& conn, const google::protobuf::Message* msg)
{
	std::shared_ptr<buffer> buff(new buffer(65536));
	const std::string& typeName = msg->GetTypeName();
	const uint16_t nameLen = static_cast<uint16_t>(typeName.size() + 1);
	const std::string message_data = msg->SerializeAsString();
	LOG(INFO)<<"[protobuf_codec],send,typename="<<typeName.c_str();
	//TODO: hton
	buff->append(&nameLen, sizeof(nameLen));
	buff->append(typeName.c_str(), nameLen);
	buff->append(message_data.c_str(), message_data.size());
	conn->send_buffer(buff);
}

void protobuf_codec::send(conn_weak_ptr& conn, const MessagePtr& msg)
{
	send(conn, msg.get());
}

void protobuf_codec::send(conn_weak_ptr& c, const google::protobuf::Message* msg)
{
	if (auto conn = c.lock())
	{
		send(conn, msg);
	}
}
}
