#include <asio.hpp>
#include <asio/basic_socket_acceptor.hpp>
#include <asio/io_context.hpp>
#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/placeholders.hpp>
#include <asio/write.hpp>
#include <exception>
#include <iostream>
#include <memory>

#include "async_tcp.h"

int main() {
  try {
    asio::io_context io;
    tcp_server server(io);
    io.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}