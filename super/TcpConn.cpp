#include "TcpConn.h"
#include "Proto.h"
#include "NetMisc.h"

using namespace loki;

uint32_t tcp_connection::s_id = 0;

bool tcp_connection::ParseBuffer()
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
