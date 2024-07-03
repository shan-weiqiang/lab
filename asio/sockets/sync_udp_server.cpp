#include <array>
#include <asio.hpp>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <iostream>

std::string make_daytime_string() {
  std::time_t now = time(0);
  return ctime(&now);
}
int main(int argc, char *argv[]) {
  using asio::ip::udp;

  try {
    asio::io_context io_context;
    udp::socket socket_(io_context, udp::endpoint(udp::v4(), 5002));
    for (;;) {
      std::array<char, 1> recv_buf;
      udp::endpoint remote_endpoint;
      socket_.receive_from(asio::buffer(recv_buf), remote_endpoint);
      std::string message = make_daytime_string();
      std::error_code error;
      socket_.send_to(asio::buffer(message), remote_endpoint, 0, error);
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}