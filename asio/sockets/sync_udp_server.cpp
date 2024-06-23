#include <array>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>

std::string make_daytime_string() {
  std::time_t now = time(0);
  return ctime(&now);
}
int main(int argc, char *argv[]) {
  using boost::asio::ip::udp;

  try {
    boost::asio::io_context io_context;
    udp::socket socket_(io_context, udp::endpoint(udp::v4(), 5002));
    for (;;) {
      std::array<char, 1> recv_buf;
      udp::endpoint remote_endpoint;
      socket_.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
      std::string message = make_daytime_string();
      boost::system::error_code error;
      socket_.send_to(boost::asio::buffer(message), remote_endpoint, 0, error);
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}