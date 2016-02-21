#include "SubService.h"
#include "super.pb.h"
#include <boost/lexical_cast.hpp>
#include "compressor_zlib.h"
#include "TcpConn.h"

namespace loki
{

SubService* SubService::sub_instance_ = nullptr;

SubService::SubService(io_service_pool& pool, const uint32_t server_type, const std::string& script_file)
	:service(pool),
	type_(server_type),
	script_file_(script_file)
{
	sub_instance_ = this;
}

void SubService::ExecuteMsg(TcpConnPtr conn, MessagePtr msg)
{
	get_logic_service().post(boost::bind(&ProtoDispatcher::onProtobufMessage, &dispatcher_, conn, msg));
}

void SubService::HandleError(TcpConnPtr conn, const boost::system::error_code& err)
{
	get_logic_service().post(boost::bind(&SubService::SyncHandleError, this, conn, err));
}

//{ from super
static bool onStartup_Response(TcpConnPtr conn, const std::shared_ptr<Super::t_Startup_Response> msg)
{
	const uint32_t size = msg->entry_size();
	if (msg->index() >= size)
	{
		LOG(ERROR)<<msg->GetTypeName()<<" data error";
		return false;
	}
	for (size_t i=0;i<size; ++i)
	{
		ServerEntryPtr entry(new Super::ServerEntry(msg->entry(i)));
		SubService::get_instance().add_entry(entry);
		if (i == msg->index())
			SubService::get_instance().set_server_entry(entry);
	}
	if (!SubService::get_instance().started())
	{
		if (!SubService::get_instance().StartServer())
		{
			LOG(ERROR)<<"start failed";
		}
	}
	else
	{
		LOG(INFO)<<"recieve server entry:"<<conn->GetRemoteIP();
	}
	return true;
}

bool SubService::Init()
{
	setup_signal_callback();

	script_.reset(new script());
	if (!script_->dofile(script_file_))
	{
		return false;
	}

	lua_tinker::table commontable = script_->get<lua_tinker::table>("Common");
	//as client
	superclient_.reset(new TcpConnection(pool_));
	superclient_->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &superDispatcher_, std::placeholders::_1, std::placeholders::_2);
	superclient_->errorHandler = std::bind(&SubService::disconnectSuper, this, std::placeholders::_1, std::placeholders::_2);

	superclient_->connectedHandler = std::bind(&SubService::handleConnectSuper, this, std::placeholders::_1, std::placeholders::_2);
	superclient_->AsyncConnect(commontable.get<const char*>("super_ip"), commontable.get<uint16_t>("super_port"));

	///Register super msg
	superDispatcher_.registerMsgCallback<Super::t_Startup_Response>(std::bind(onStartup_Response, std::placeholders::_1, std::placeholders::_2));    //登录消息返回
	///

	_registerScript(script_->get_state());
	return true;
}

void SubService::disconnectSuper(TcpConnPtr conn, const boost::system::error_code& error)
{
	handle_stop();
}

void SubService::handleConnectSuper(TcpConnPtr conn, const boost::system::error_code& error)
{
	if (!error)
	{
		LOG(INFO)<<"Connected to Server success : "<<conn->GetRemoteIP();
		Super::t_Startup_Request send;
		send.set_type(get_type());
		superclient_->SendMessage(&send);
	}
	else
	{
		LOG(INFO)<<"Connected to Server failed : "<<conn->GetRemoteIP();
		handle_stop();
	}
}

bool SubService::StartServer()
{
	LOG(INFO)<<"Start Sub Server";
	RegisterCallback();
	LOG(INFO)<<"RegisterCallback size = "<<dispatcher_.size();
	try {
		server_.reset(new TcpServer(pool_, get_ip(), boost::lexical_cast<std::string>(get_port())));
	}
	catch(...)
	{
		LOG(ERROR)<<"Create TcpServer Failed, normally it's that the port is in use";
		return false;
	}
	//server_->msgHandler = std::bind(&ProtoDispatcher::onProtobufMessage, &dispatcher_, std::placeholders::_1, std::placeholders::_2);
	//synchronize process message
	server_->msgHandler = std::bind(&SubService::ExecuteMsg, this, std::placeholders::_1, std::placeholders::_2);
	server_->errorHandler = std::bind(&SubService::HandleError, this, std::placeholders::_1, std::placeholders::_2);
	server_->start_accept();

	started_ = true;
	return PostInit();
}

SubService::~SubService()
{
}

static SubService* get_SubService() 
{
	return &(SubService::get_instance());
}

void SubService::_registerScript(lua_State* L)
{
	lua_tinker::def(L, "get_SubService", get_SubService);
	lua_tinker::class_add<SubService>(L, "SubService");
	lua_tinker::class_def<SubService>(L, "get_id", &SubService::get_id);
	lua_tinker::class_def<SubService>(L, "get_type", &SubService::get_type);
	registerScript(L);
}

}
