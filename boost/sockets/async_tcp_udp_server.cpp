#include "async_tcp.h"
#include "async_udp.h"
#include <boost/asio/io_context.hpp>
#include <exception>
#include <iostream>

int main() {
  try {
    boost::asio::io_context io_context;
    tcp_server server1(io_context);
    udp_server server2(io_context);
    io_context.run();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}