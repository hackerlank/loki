#include "GatewayTask.h"
#include "compressor_zlib.h"
#include "encryptor_aes.h"
#include "protobuf_codec.h"
#include "logger.h"
#include "Session.pb.h"
#include "GatewayServer.h"

GatewayTask::GatewayTask(connection_ptr conn)
	:context(conn), ip_(conn->get_peer_ip())
{
	char key[32]= {'a', 'b', 'c'};
	std::shared_ptr<loki::encryptor_aes> aes(new loki::encryptor_aes());
	aes->setkey((const unsigned char*)key,256);
	conn->set_encryptor(aes);
	conn->set_compressor(std::shared_ptr<loki::compressor>(new loki::compressor_zlib()));
}

GatewayTask::~GatewayTask()
{
	LOG(INFO)<<__func__;
}

void GatewayTask::refreshCharInfo()
{
	Gate::stUserInfoUserCmd send;
	for(size_t i = 0;i<user_info_->info_size(); ++i)
	{
		auto info = send.add_charinfo();
		info->CopyFrom(user_info_->info(i));
	}
	protobuf_codec::send(connection(), &send);
	selectState();
	LOG(INFO)<<__func__<<",charinfo size = "<<user_info_->info_size();
}

bool GatewayTask::charInfoFull()
{
	return (user_info_ && user_info_->info_size() >= 2);
}

void GatewayTask::reg(const uint32_t no)
{
	Session::t_regUser_GateSession send;
	send.set_accid(accid());
	send.set_id(id());
	send.set_country(country());
	send.set_name(name());
	send.set_ip(ip());
	protobuf_codec::send(GatewayServer::instance().sessionclient_, &send);
	setState(Systemstate::SYSTEM_STATE_WAIT_PLAY);
}
