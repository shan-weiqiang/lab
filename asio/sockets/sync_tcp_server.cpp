#include <asio.hpp>
#include <asio/io_context.hpp>
#include <asio/write.hpp>
#include <ctime>
#include <iostream>

using asio::ip::tcp;

std::string make_daytime_string() {
  std::time_t now = time(0);
  return ctime(&now);
}

int main() {
  try {
    asio::io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));
    for (;;) {
      tcp::socket socket(io);
      acceptor.accept(socket);
      std::string message = make_daytime_string();
      std::error_code error;
      asio::write(socket, asio::buffer(message), error);
      std::cout << "time sent to client, time is: " << message << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}