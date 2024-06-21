#include <array>
#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <exception>
#include <iostream>

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "usage: client <host>" << std::endl;
      return 1;
    }
    boost::asio::io_context io;
    tcp::resolver res(io);
    tcp::resolver::results_type endpoints = res.resolve(argv[1], "5002");
    tcp::socket socket(io);
    boost::asio::connect(socket, endpoints);
    for (;;) {
      std::array<char, 128> buf;
      boost::system::error_code error;
      size_t len = socket.read_some(boost::asio::buffer(buf), error);
      if (error == boost::asio::error::eof) {
        break;
      } else if (error) {
        throw boost::system::system_error(error);
      }
      std::cout.write(buf.data(), len);
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}