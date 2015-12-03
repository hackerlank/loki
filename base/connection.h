#pragma once

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/any.hpp>
#include <functional>
#include <string>
#include "buffer.h"
#include "allocator.h"
#include <list>
#include "mutex.h"

namespace loki
{

const uint32_t MSG_NAME_LENGTH_SIZE = 2;
const uint32_t MIN_MSG_NAME_SIZE = 4;
const uint32_t MSG_HEADER_LENGTH = 4;
const uint32_t MAX_MSG_LENGTH = 65535;

class connection;
typedef std::shared_ptr<connection> connection_ptr;
typedef std::weak_ptr<connection> conn_weak_ptr;
class compressor;
class encryptor;

/// Represents a single connection from a client.
class connection
  : public std::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
  enum package_flag
  {
	  PACKET_ZLIB = 1<<31,		//压缩
	  PACKET_ENC  = 1<<30,		//机密
  };
  static bool is_compress(const uint32_t n) { return n & PACKET_ZLIB; }
  static bool is_encrypt(const uint32_t n) { return n & PACKET_ENC; }
  /// Construct a connection with the given io_service.
  explicit connection(boost::asio::io_service& io_service, const std::string name = "client");
  virtual ~connection();

  boost::asio::ip::tcp::socket& socket() { return socket_; }
  boost::asio::io_service& get_io_service() { return socket_.get_io_service(); }

  void async_connect(const std::string& host, const uint16_t port);
  bool connect(const std::string& host, const uint16_t port);
  void handle_connect(const boost::system::error_code& error);

  void async_read();

  /// Start the first asynchronous operation for the connection.
  void start();

  void set_connected_handler(std::function<void (connection_ptr)> cb)
  {
	  connected_handler_ = cb;
  }
  void set_msg_handler(std::function<void (connection_ptr)> cb)
  {
	  msg_handler_ = cb;
  }

  void set_error_handler(std::function<void (connection_ptr)> cb)
  {
	  error_handler_ = cb;
  }

  void send_buffer(std::shared_ptr<buffer>& buf);
  void send_buffer_in_queue(std::shared_ptr<buffer> buf);

  void send_raw_buffer(std::shared_ptr<buffer>& buf);
  void send_raw_buffer_in_queue(std::shared_ptr<buffer> buf);

  void send(const char* msg, const size_t len);


  const std::string get_peer_ip();
  uint16_t get_peer_port();
  const std::string& get_peer_info() const { return peer_info_; }

  const uint32_t get_id() const { return id_; }
  void set_id(const uint32_t id) { id_ = id; }

  bool is_connected() const { return connected_; }

  buffer* get_buffer() { return &recv_buffer_; }

  void shutdown();
  void close();

  void set_context(const boost::any& context) { context_ = context; }
  const boost::any& get_context() const { return context_; }
  boost::any* get_mutable_context() { return &context_; }
  boost::any release_context() { boost::any temp; boost::swap(temp, context_); return temp; }

  void set_reuse(bool b) { reuse_ = b; }
  void set_compressor(std::shared_ptr<compressor> c) { compressor_ = c; }
  void set_encryptor(std::shared_ptr<encryptor> e) { encryptor_ = e; }
  bool restore_msg(const char* ptr, size_t flag, char* out, size_t& len);

  void clear_handle();
  const std::string& name() const { return name_; }
protected:
  void _send_raw_in_queue(const char* ptr, const size_t size);
  //mainly represent data associated with the connection
  boost::any context_;		
  handler_allocator allocator_;
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e, std::size_t bytes_transferred);
  void handle_write2(const boost::system::error_code& e, std::size_t bytes);
  void handle_error(const boost::system::error_code& e, const std::string& info);

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  std::function<void (connection_ptr)> connected_handler_;
  std::function<void (connection_ptr)> msg_handler_;
  std::function<void (connection_ptr)> error_handler_;

  std::shared_ptr<compressor> compressor_;
  std::shared_ptr<encryptor>  encryptor_;

  buffer recv_buffer_;
  buffer send_buffer_;

  int flag_{-1};

  bool connected_{false};
  bool reuse_{false};
  std::string peer_info_;
  std::string id_str_;
  uint32_t id_{0};
  std::string name_{"client"};

  std::mutex mutex_;
  std::list<std::shared_ptr<buffer> > buffers_;
};

} // namespace server

