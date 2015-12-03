#include "connection.h"
#include <vector>
#include <boost/bind.hpp>
#include "logger.h"
#include "compressor.h"
#include "encryptor.h"
#include <boost/lexical_cast.hpp>

namespace loki 
{

static uint32_t s_count = 0;

connection::connection(boost::asio::io_service& io_service, const std::string name)
  : socket_(io_service),
	recv_buffer_(65536),
	send_buffer_(65536),
	id_(++s_count),
	name_(name)
{
	char buff[50];
	snprintf(buff, sizeof(buff),"%u", get_id());
	id_str_ = buff;
}

//run in io thread
void connection::start()
{
	connected_ = true;
	flag_ = 0;
	//this must be called after connection is accepted, otherwise it will throw.
	boost::asio::socket_base::keep_alive option1(true);
	socket_.set_option(option1);

	boost::asio::ip::tcp::no_delay option(true);
	socket_.set_option(option);

	if (connected_handler_)
		connected_handler_(shared_from_this());
	async_read();

	peer_info_ = "id=" + id_str_+ ",ip=" + get_peer_ip() + ",port=" + boost::lexical_cast<std::string>(get_peer_port());
	LOG(INFO)<<"start: "<<peer_info_;
}

void connection::async_read()
{
	socket_.async_read_some(boost::asio::buffer(recv_buffer_.free_data()),
			make_custom_alloc_handler(allocator_,
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)));
}

//run in io thread
void connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
{
	if (!e)
	{
		recv_buffer_.add_data_size(bytes_transferred);

		if (msg_handler_)
			msg_handler_(shared_from_this());
		async_read();
	}
	else
	{
		handle_error(e, __func__);
	}
}

//run in io thread
void connection::handle_write(const boost::system::error_code& e, std::size_t bytes_transferred)
{
  if (!e)
  {
	  send_buffer_.consume(bytes_transferred);
	  if (send_buffer_.data_size() > 0)
	  {
		   socket_.async_write_some(boost::asio::buffer(send_buffer_.data()),
				   make_custom_alloc_handler(allocator_,
				   boost::bind(&connection::handle_write, shared_from_this(),_1,_2)));
	  }
  }
  else
  {
	  handle_error(e, __func__);
  }
}

void connection::handle_write2(const boost::system::error_code& e, std::size_t bytes)
{
  if (!e)
  {
	  std::shared_ptr<buffer> buf;
	  {
		  scoped_lock lock(mutex_);
		  buffers_.pop_front();
		  if (!buffers_.empty())
		  {
			  buf = buffers_.front();
		  }
	  }
	  //get a valid buffer
	  if (buf)
		  boost::asio::async_write(socket_, boost::asio::buffer(buf->data()), boost::bind(&connection::handle_write2, shared_from_this(), _1, _2));
  }
  else
  {
	  handle_error(e, __func__);
  }
}

//run in io thread
void connection::handle_error(const boost::system::error_code& e, const std::string& info)
{
	LOG(INFO)<<__func__<<",id:"<<id_str_<<", info:"<<info<<",flag:"<<flag_++<<", err_msg:"<<e.message()<<",err_value="<<e.value();

	if (flag_ == 1)
	{
		if (error_handler_)
			error_handler_(shared_from_this());
		close();
	}

	connected_ = false;
}

void connection::clear_handle()
{
	decltype(error_handler_) null_func;
	error_handler_ = null_func;
	connected_handler_ = null_func;
	msg_handler_ = null_func;
}

void connection::send_buffer(std::shared_ptr<buffer>& buf)
{
	get_io_service().post(boost::bind(&connection::send_buffer_in_queue, shared_from_this(), buf));
}

void connection::send_buffer_in_queue(std::shared_ptr<buffer> buf)
{
	const void* msg = buf->data_ptr();
	const size_t n = buf->data_size();
	assert (connected_);
	if (n > 0)
	{
		bool zip = false;
		bool enc = false;

		char zipbuf[65536];
		size_t ziplen = sizeof(zipbuf);
		std::pair<const char*, uint32_t> zip_pair(static_cast<const char*>(msg), n);
		if (compressor_ && n > 64)
		{
			if (!compressor_->compress(static_cast<const char*>(msg), n, zipbuf, ziplen))
			{
				LOG(ERROR)<<__func__<<",id="<<id_str_<<",compress error,n="<<n;
				close();
				return ;
			}
			LOG(INFO)<<__func__<<",id="<<id_str_<<",compress,len="<<n<<",ziplen="<<ziplen;
			zip_pair.first = zipbuf;
			zip_pair.second = ziplen;
			zip = true;
		}

		char enc_buf[65536];
		size_t enc_len = sizeof(enc_buf);
		std::pair<const char*, uint32_t> enc_pair(zip_pair);
		if (encryptor_)
		{
			if (!encryptor_->encrypt(zip_pair.first, zip_pair.second, enc_buf, enc_len))
			{
				close();
				return ;
			}
			LOG(INFO)<<__func__<<",id="<<id_str_<<",encrypt,len="<<zip_pair.second<<",enclen="<<enc_len;
			enc_pair.first = enc_buf;
			enc_pair.second = enc_len;
			enc = true;
		}
		uint32_t flag = enc_pair.second;
		if (zip) flag |= PACKET_ZLIB;
		if (enc) flag |= PACKET_ENC;
		bool sending = send_buffer_.data_size() > 0;
		send_buffer_.append(&flag, sizeof(uint32_t));
		send_buffer_.append(enc_pair.first, enc_pair.second);
		if (!sending)
		{
			socket_.async_write_some(boost::asio::buffer(send_buffer_.data()), 
					boost::bind(&connection::handle_write, shared_from_this(), _1, _2));
		}
	}
}

void connection::send(const char* msg, const size_t len)
{
	std::shared_ptr<buffer> buff(new buffer(len));
	buff->append(msg, len);
	bool can_send = false;
	{
		//keep buffer alive and valid
		scoped_lock lock(mutex_);
		can_send = buffers_.empty();
		buffers_.push_back(buff);
	}
	if (can_send)
		boost::asio::async_write(socket_, boost::asio::buffer(buff->data()), boost::bind(&connection::handle_write2, shared_from_this(),_1, _2));
}

void connection::_send_raw_in_queue(const char* ptr, const size_t size)
{
	assert (connected_);
	if (size > 0)
	{
		bool sending = send_buffer_.data_size() > 0;
		send_buffer_.append(ptr, size);
		if (!sending)
		{
			socket_.async_write_some(boost::asio::buffer(send_buffer_.data()), 
					boost::bind(&connection::handle_write, shared_from_this(), _1, _2));
		}
	}
}

void connection::send_raw_buffer(std::shared_ptr<buffer>& buf)
{
	get_io_service().post(boost::bind(&connection::send_raw_buffer_in_queue, shared_from_this(),buf));
}

void connection::send_raw_buffer_in_queue(std::shared_ptr<buffer> buf)
{
	_send_raw_in_queue((const char*)buf->data_ptr(), buf->data_size());
}

const std::string connection::get_peer_ip()
{
	boost::system::error_code ec;
	boost::asio::ip::tcp::socket::endpoint_type endpoint = socket_.remote_endpoint(ec);
	if (ec)
		    return "";
	else {
		    return endpoint.address().to_string();
	}
}

uint16_t connection::get_peer_port()
{
	boost::system::error_code ec;
	boost::asio::ip::tcp::socket::endpoint_type endpoint = socket_.remote_endpoint(ec);
	if (ec)
		    return 0;
	else {
		    return endpoint.port();
	}

}

connection::~connection()
{
	LOG(INFO)<<__func__<<",id="<<id_str_<<",name="<<name_;
}

void connection::shutdown()
{
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
}

void connection::close()
{
    boost::system::error_code ec;
	socket_.close(ec);
}

void connection::async_connect(const std::string& host, const uint16_t port)
{
	LOG(INFO)<<__func__<<",host="<<host<<",port="<<port;
	flag_ = 0;
	using namespace boost::asio::ip;
	tcp::resolver resolver(get_io_service());
	tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
	tcp::resolver::iterator iterator = resolver.resolve(query);

	boost::asio::async_connect(socket_, iterator, boost::bind(&connection::handle_connect, shared_from_this(), boost::asio::placeholders::error));
}

void connection::handle_connect(const boost::system::error_code& e)
{
	if (!e)
	{
		start();
	}
	else
	{
		handle_error(e, __func__);
	}
}

bool connection::connect(const std::string& ip, const uint16_t port)
{
	flag_ = 0;
	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint endpoint(ip::address::from_string(ip.c_str()), port);
	socket_.connect(endpoint, ec);
	if (ec) return false;
	else
	{	
		start();
		return true;
	}
}

bool connection::restore_msg(const char* ptr, size_t flag, char* buf, size_t& len)
{
	const size_t msg_len = flag & 0x0000ffff;
	
	if (is_encrypt(flag))
	{
		char dec_buf[MAX_MSG_LENGTH];
		size_t dec_len = sizeof(dec_buf);
		std::pair<const char*, uint32_t> dec_pair;

		assert (encryptor_);
		if (!is_compress(flag))
		{
			if (!encryptor_->decrypt(ptr, msg_len, buf, len))
			{
				LOG(ERROR)<<__func__<<",connection="<<get_peer_info()<<", decodec";
				return false;
			}
			return true;
		}
		else
		{
			assert (compressor_);
			if (!encryptor_->decrypt(ptr, msg_len, dec_buf, dec_len))
			{
				LOG(ERROR)<<__func__<<",id="<<get_peer_info()<<", decodec 2";
				return false;
			}
			if (!compressor_->uncompress(dec_buf, dec_len, buf, len))
			{
				LOG(ERROR)<<__func__<<"id="<<get_peer_info()<<", uncompress";
				return false;
			}
			return true;
		}
	}
	else
	{
		if (!is_compress(flag))
		{
			memcpy(buf, ptr, msg_len);
			len = msg_len;
			return true;
		}
		else
		{
			assert (compressor_);
			if (!compressor_->uncompress(ptr, msg_len, buf, len))
			{
				LOG(ERROR)<<__func__<<"id="<<get_peer_info()<<", uncompress2";
				return false;
			}
			return true;
		}
	}
}

} 


