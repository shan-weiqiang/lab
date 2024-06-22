#include "make_day_time.h"
#include <boost/asio.hpp>
#include <memory>

#ifndef ASYNC_TCP
#define ASYNC_TCP

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
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
  tcp_server(boost::asio::io_service &io)
      : io{io}, acceptor{io, boost::asio::ip::tcp::endpoint(
                                 boost::asio::ip::tcp::v4(), 5002)} {
    start_accept();
  }

  void start_accept() {
    tcp_connection::pointer new_con = tcp_connection::create(io);
    acceptor.async_accept(new_con->socket(),
                          std::bind(&tcp_server::handle_accept, this, new_con));
  }
  void handle_accept(tcp_connection::pointer new_connection) {
    new_connection->start();
    start_accept();
  }

private:
  boost::asio::io_context &io;
  boost::asio::ip::tcp::acceptor acceptor;
};

#endif