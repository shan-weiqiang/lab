#include <asio.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <iostream>

int main() {
  asio::io_context io;
  asio::steady_timer t(io, asio::chrono::seconds(5));
  t.wait();
  std::cout << "Hello World!" << std::endl;
  return 0;
}