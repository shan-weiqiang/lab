#include "make_day_time.h"
#include <asio.hpp>
#include <memory>
#include <functional>

#ifndef ASYNC_TCP
#define ASYNC_TCP

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
  typedef std::shared_ptr<tcp_connection> pointer;
  static pointer create(asio::io_context &io) {
    return pointer(new tcp_connection(io));
  }
  asio::ip::tcp::socket &socket() { return socket_; }

  void start() {
    message_ = make_daytime_string();
    asio::async_write(
        socket_, asio::buffer(message_),
        std::bind(&tcp_connection::handle_write, shared_from_this()));
  }

private:
  tcp_connection(asio::io_context &io) : socket_{io} {}
  void handle_write() {}
  asio::ip::tcp::socket socket_;
  std::string message_;
};
class tcp_server {
public:
  tcp_server(asio::io_service &io)
      : io{io}, acceptor{io, asio::ip::tcp::endpoint(
                                 asio::ip::tcp::v4(), 5002)} {
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
  asio::io_context &io;
  asio::ip::tcp::acceptor acceptor;
};

#endif