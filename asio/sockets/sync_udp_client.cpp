#include <array>
#include <asio.hpp>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <iostream>
using asio::ip::udp;

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "usage: client <host>" << std::endl;
      return 1;
    }
    asio::io_context io_context;
    udp::resolver resolver(io_context);
    udp::endpoint receiver_endpoint =
        *resolver.resolve(udp::v4(), argv[1], "5002").begin();
    udp::socket socket_(io_context);
    socket_.open(udp::v4());
    std::array<char, 1> send_buf = {0};
    socket_.send_to(asio::buffer(send_buf), receiver_endpoint);

    std::array<char, 128> recv_buf;
    udp::endpoint sender_point;
    size_t len =
        socket_.receive_from(asio::buffer(recv_buf), sender_point);
    std::cout.write(recv_buf.data(), len);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
