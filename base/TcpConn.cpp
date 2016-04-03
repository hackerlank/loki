#include "TcpConn.h"
#include "Proto.h"
#include "NetMisc.h"
#include "buffer.h"
#include "Service.h"

using namespace loki;

uint32_t TcpConnection::s_id = 0;

#if 0
bool TcpConnection::ParseBuffer()
{
	if (Package::IsEncrypt(msgHeader.flag))
	{
		LOG(INFO)<<"Message is encrypted";
		if (!encryptor_)
			return false;
		std::vector<char> buf(65535);
		size_t bufLen = buf.size();
		if (!encryptor_->decrypt(&(msgBuf[0]), msgBuf.size(), buf.data(), bufLen))
			return false;
		buf.resize(bufLen);
		buf.swap(msgBuf);
	}
	if (Package::IsCompress(msgHeader.flag))
	{
		LOG(INFO)<<"Message is compressed";
		std::vector<char> buf(65535);
		size_t bufLen = buf.size();
		if (!NetMisc::uncompress(&(msgBuf[0]), msgBuf.size(), buf.data(), bufLen))
			return false;
		buf.resize(bufLen);
		buf.swap(msgBuf);
	}
	const uint16_t msgNameLen = *(const uint16_t*)(msgBuf.data());
	if (msgNameLen >= Package::MsgNameMaxSize || msgNameLen + Package::MsgNameLengthSize > msgBuf.size())
		return false;
	char msgName[Package::MsgNameMaxSize] = {'\0'};
	strncpy(msgName, msgBuf.data() + Package::MsgNameLengthSize, msgNameLen);
	const uint32_t n = strlen(msgName) + 1;
	if (n != msgNameLen)
		return false;
	LOG(INFO)<<"Msg name = "<<msgName;
	MessagePtr message(Proto::CreateMessage(msgName));
	if (!message)
		return false;
	if (!message->ParseFromString(std::string(msgBuf.data() + Package::MsgNameLengthSize + msgNameLen, msgBuf.size() - Package::MsgNameLengthSize - msgNameLen)))
		return false;

	if (msgHandler)
		msgHandler(shared_from_this(), message);
	return true;
}

// A reference-counted non-modifiable buffer class.
class shared_const_buffer
{
	public:
		// Construct from a std::string.
		explicit shared_const_buffer(const std::string& data)
			: data_(new std::vector<char>(data.begin(), data.end())),
			buffer_(boost::asio::buffer(*data_))
	{
	}

		// Implement the ConstBufferSequence requirements.
		typedef boost::asio::const_buffer value_type;
		typedef const boost::asio::const_buffer* const_iterator;
		const boost::asio::const_buffer* begin() const { return &buffer_; }
		const boost::asio::const_buffer* end() const { return &buffer_ + 1; }

	private:
		boost::shared_ptr<std::vector<char> > data_;
		boost::asio::const_buffer buffer_;
};

void TcpConnection::SendMessage(const MessagePtr msg)
{
	SendMessage(msg.get());
}

void TcpConnection::SendMessage(const google::protobuf::Message* msg)
{
	std::shared_ptr<buffer> buff(new buffer(65536));
	const std::string& typeName = msg->GetTypeName();
	const uint16_t nameLen = static_cast<uint16_t>(typeName.size() + 1);
	const std::string message_data = msg->SerializeAsString();
	LOG(INFO)<<__func__<<", typename="<<typeName.c_str();
	//TODO: hton
	uint32_t placeHolder = 0;
	buff->append(&placeHolder, sizeof(placeHolder));
	buff->append(&nameLen, sizeof(nameLen));
	buff->append(typeName.c_str(), nameLen);
	buff->append(message_data.c_str(), message_data.size());
	uint32_t* pLength = static_cast<uint32_t*>(buff->data_ptr());
	*pLength = static_cast<uint32_t>(buff->data_size() - 4);

	shared_const_buffer bbb(std::string((const char*)buff->data_ptr(), buff->data_size()));
	boost::asio::async_write(socket_, bbb, boost::bind(&TcpConnection::handleWrite, shared_from_this(), _1, _2));
}
void TcpConnection::handleWrite(const boost::system::error_code& error, size_t /*bytes_transferred*/)
{
	if (error)
		handleError(error);
}

#endif

bool TcpConnection::ParseBuffer()
{
	if (Package::IsEncrypt(msgHeader.flag))
	{
		LOG(INFO)<<"Message is encrypted";
		if (!encryptor_)
			return false;
		std::vector<char> buf(65535);
		size_t bufLen = buf.size();
		if (!encryptor_->decrypt(&(msgBuf[0]), msgBuf.size(), buf.data(), bufLen))
			return false;
		buf.resize(bufLen);
		buf.swap(msgBuf);
	}
	if (Package::IsCompress(msgHeader.flag))
	{
		LOG(INFO)<<"Message is compressed";
		std::vector<char> buf(65535);
		size_t bufLen = buf.size();
		if (!NetMisc::uncompress(&(msgBuf[0]), msgBuf.size(), buf.data(), bufLen))
			return false;
		buf.resize(bufLen);
		buf.swap(msgBuf);
	}
	const uint32_t msgId = *(const uint32_t*)(msgBuf.data());
	//LOG(INFO)<<"Msg id = "<<msgId;
	std::string msgName(lua_tinker::call<const char*>(Service::Instance()->GetLuaState(), "GetMessageNameByID", msgId));
	if (msgName.empty())
	{
		LOG(INFO)<<"Unknown msgID = "<<msgId;
		return false;
	}
	uint32_t protoBuffLen = *(const uint32_t*)(msgBuf.data() + sizeof(msgId));
	if (msgBuf.size() != static_cast<size_t>(sizeof(msgId) + sizeof(protoBuffLen) + protoBuffLen))
	{
		LOG(INFO)<<"Receive Msg Length = "<<msgBuf.size()<<", protoBufLen = "<<protoBuffLen;
		return false;
	}
	//LOG(INFO)<<"Msg name = "<<msgName;
	MessagePtr message(Proto::CreateMessage(msgName));
	if (!message)
		return false;
	if (!message->ParseFromString(std::string(msgBuf.data() + sizeof(msgId) + sizeof(protoBuffLen), msgBuf.size() - sizeof(msgId) - sizeof(protoBuffLen))))
		return false;

	if (msgHandler)
		msgHandler(shared_from_this(), message);
	return true;
}

// A reference-counted non-modifiable buffer class.
class shared_const_buffer
{
	public:
		// Construct from a std::string.
		explicit shared_const_buffer(const std::string& data)
			: data_(new std::vector<char>(data.begin(), data.end())),
			buffer_(boost::asio::buffer(*data_))
	{
	}

		// Implement the ConstBufferSequence requirements.
		typedef boost::asio::const_buffer value_type;
		typedef const boost::asio::const_buffer* const_iterator;
		const boost::asio::const_buffer* begin() const { return &buffer_; }
		const boost::asio::const_buffer* end() const { return &buffer_ + 1; }

	private:
		boost::shared_ptr<std::vector<char> > data_;
		boost::asio::const_buffer buffer_;
};

void TcpConnection::SendMessage(const MessagePtr msg)
{
	SendMessage(msg.get());
}

//message format
// |4byte message length, not include this 4 byte| 2byte message id| 4 byte protobuf message serialize length| xxxxxx|
//
void TcpConnection::SendMessage(const google::protobuf::Message* msg)
{
	if (!Connected()) 
	{
		LOG(INFO)<<"SendMessage in Unconnected socket!!!";
		return;
	}
	std::shared_ptr<buffer> buff(new buffer(65536));
	const std::string& typeName = msg->GetTypeName();
	const uint32_t msgId = lua_tinker::call<const uint32_t>(Service::Instance()->GetLuaState(), "GetMessageIDByName", typeName.c_str());
	if (msgId == 0)
	{
		LOG(INFO)<<"SendMessage Error Unregister Message "<<typeName;
		return ;
	}
	const std::string message_data = msg->SerializeAsString();
	LOG(INFO)<<__func__<<", typename="<<typeName.c_str();
	//TODO: hton
	uint32_t placeHolder = 0;
	buff->append(&placeHolder, sizeof(placeHolder));
	buff->append(&msgId, sizeof(msgId));
	uint32_t protoBuffLength = message_data.size();
	buff->append(&protoBuffLength, sizeof(protoBuffLength));
	buff->append(message_data.c_str(), message_data.size());
	uint32_t* pLength = static_cast<uint32_t*>(buff->data_ptr());
	*pLength = static_cast<uint32_t>(buff->data_size() - 4);

	shared_const_buffer bbb(std::string((const char*)buff->data_ptr(), buff->data_size()));
	boost::asio::async_write(socket_, bbb, boost::bind(&TcpConnection::handleWrite, shared_from_this(), _1, _2));
}
void TcpConnection::handleWrite(const boost::system::error_code& error, size_t /*bytes_transferred*/)
{
	if (error)
		handleError(error, "write");
}
