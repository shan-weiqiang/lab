#include <boost/asio.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <exception>
#include <iostream>
#include <memory>

#include "async_tcp.h"

int main() {
  try {
    boost::asio::io_context io;
    tcp_server server(io);
    io.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}