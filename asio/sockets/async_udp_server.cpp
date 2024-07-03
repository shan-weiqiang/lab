#include <array>
#include <asio.hpp>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#include "async_udp.h"

int main() {
  try {
    asio::io_context io_context;
    udp_server server(io_context);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
