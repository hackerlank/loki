#include "sub_service.h"
#include "super.pb.h"
#include <boost/lexical_cast.hpp>
#include "compressor_zlib.h"

namespace loki
{

sub_service* sub_service::sub_instance_ = nullptr;

sub_service::sub_service(io_service_pool& pool, const uint32_t server_type, const std::string& script_file)
	:service(pool),
	type_(server_type),
	codec_(std::bind(&sub_service::do_msg, this, std::placeholders::_1, std::placeholders::_2)),
	script_file_(script_file)
{
	sub_instance_ = this;
}

void sub_service::do_msg(connection_ptr conn, MessagePtr msg)
{
	get_logic_service().post(boost::bind(&protobuf_dispatcher::onProtobufMessage, &dispatcher_, conn, msg));
}

bool sub_service::init()
{
	setup_signal_callback();

	script_.reset(new script());
	if (!script_->dofile(script_file_))
	{
		return false;
	}

	lua_tinker::table commontable = script_->get<lua_tinker::table>("Common");
	//as client
	superclient_.reset(new connection(pool_.get_io_service(), "superclient"));
	superclient_->set_connected_handler(std::bind(&sub_service::on_super_connected, this, std::placeholders::_1));
	superclient_->set_msg_handler(std::bind(&sub_service::on_msg, this, std::placeholders::_1));
	superclient_->set_error_handler(std::bind(&sub_service::handle_stop, this));

	superclient_->async_connect(commontable.get<const char*>("super_ip"), commontable.get<uint16_t>("super_port"));
	superclient_->set_compressor(std::shared_ptr<loki::compressor>(new loki::compressor_zlib()));

	_registerCallback();
	_registerScript(script_->get_state());
	return true;
}

//连接到super后调用
bool sub_service::_start_server()
{
	//as server
	server_.reset(new server(get_ip(), boost::lexical_cast<std::string>(get_port()), pool_));
	server_->set_msg_handler(std::bind(&sub_service::on_msg, this, std::placeholders::_1));
	server_->set_connected_handler(std::bind(&sub_service::on_connected, this, std::placeholders::_1));
	server_->set_error_handler(std::bind(&sub_service::on_error, this, std::placeholders::_1));
	server_->start_accept();

	return start_server();
}

sub_service::~sub_service()
{
}

//{ from super
static bool onStartup_Response(connection_ptr conn, const std::shared_ptr<Super::t_Startup_Response>& msg)
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
		sub_service::get_instance().add_entry(entry);
		if (i == msg->index())
			sub_service::get_instance().set_server_entry(entry);
	}
	if (!sub_service::get_instance().started())
	{
		if (!sub_service::get_instance()._start_server())
		{
			LOG(ERROR)<<"start failed";
		}
	}
	else
	{
		LOG(INFO)<<"recieve server entry:"<<conn->get_peer_info();
	}
	return true;
}


void sub_service::_registerCallback()
{
	dispatcher_.registerMsgCallback<Super::t_Startup_Response>(std::bind(onStartup_Response, std::placeholders::_1, std::placeholders::_2));    //登录消息返回

	registerCallback();
}

void sub_service::on_msg(connection_ptr conn)
{
	codec_.handle_message(conn);
}

void sub_service::on_super_connected(connection_ptr conn)
{
	Super::t_Startup_Request send;
	send.set_type(get_type());
	codec_.send(conn, &send);
}

static sub_service* get_sub_service() 
{
	return &(sub_service::get_instance());
}

void sub_service::_registerScript(lua_State* L)
{
	lua_tinker::def(L, "get_sub_service", get_sub_service);
	lua_tinker::class_add<sub_service>(L, "sub_service");
	lua_tinker::class_def<sub_service>(L, "get_id", &sub_service::get_id);
	lua_tinker::class_def<sub_service>(L, "get_type", &sub_service::get_type);
	registerScript(L);
}

}
