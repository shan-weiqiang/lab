#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <memory>

std::string make_daytime_string() {
  std::time_t now = time(0);
  return ctime(&now);
}

class tcp_connection : std::enable_shared_from_this<tcp_connection> {
public:
  typedef std::shared_ptr<tcp_connection> pointer;
  static pointer create(boost::asio::io_context &io) {
    return pointer(new tcp_connection(io));
  }
  boost::asio::ip::tcp::socket &socket() { return socket_; }

  void start() {
    message_ = make_daytime_string();
    boost::asio::async_write(
        socket_, boost::asio::buffer(message_),
        std::bind(&tcp_connection::handle_write, shared_from_this()));
  }

private:
  tcp_connection(boost::asio::io_context &io) : socket_{io} {}
  void handle_write() {}
  boost::asio::ip::tcp::socket socket_;
  std::string message_;
};
class tcp_server {
public:
  tcp_server(boost::asio::io_service &io) {}
};

int main() {
  try {
    boost::asio::io_context io;
    
  }
}