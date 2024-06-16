#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <ctime>
#include <iostream>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  std::time_t now = time(0);
  return ctime(&now);
}

int main() {
  try {
    boost::asio::io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));
    for (;;) {
      tcp::socket socket(io);
      acceptor.accept(socket);
      std::string message = make_daytime_string();
      boost::system::error_code error;
      boost::asio::write(socket, boost::asio::buffer(message), error);
      std::cout << "time sent to client, time is: " << message << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}