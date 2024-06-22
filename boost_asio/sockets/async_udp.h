#include "make_day_time.h"
#include <boost/asio.hpp>
#include <memory>

#ifndef ASYNC_UDP
#define ASYNC_UDP


using boost::asio::ip::udp;
class udp_server {
public:
  udp_server(boost::asio::io_context &io_context)
      : socket_(io_context, udp::endpoint(udp::v4(), 5002)) {
    start_receive();
  }

private:
  udp::socket socket_;
  udp::endpoint remote_endpoint;
  std::array<char, 1> buffer;

  void start_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(buffer), remote_endpoint,
        [this](const boost::system::error_code &error, std::size_t) {
          if (!error) {
            std::shared_ptr<std::string> msg =
                std::make_shared<std::string>(make_daytime_string());
            socket_.async_send_to(
                boost::asio::buffer(*msg), remote_endpoint,
                [](const boost::system::error_code &error, std::size_t) {});
            start_receive();
          }
        });
  }
};

#endif