#include <array>
#include <asio.hpp>
#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <cstddef>
#include <exception>
#include <iostream>

using asio::ip::tcp;

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "usage: client <host>" << std::endl;
      return 1;
    }
    asio::io_context io;
    tcp::resolver res(io);
    tcp::resolver::results_type endpoints = res.resolve(argv[1], "5002");
    tcp::socket socket(io);
    asio::connect(socket, endpoints);
    for (;;) {
      std::array<char, 128> buf;
      std::error_code error;
      size_t len = socket.read_some(asio::buffer(buf), error);
      if (error == asio::error::eof) {
        break;
      } else if (error) {
        throw std::system_error(error);
      }
      std::cout.write(buf.data(), len);
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}