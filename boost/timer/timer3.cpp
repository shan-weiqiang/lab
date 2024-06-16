#include <boost/asio.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>
#include <functional>
#include <iostream>

void print(const boost::system::error_code ec, boost::asio::steady_timer *t,
           int *cnt) {
  if (*cnt < 5) {
    std::cout << *cnt << std::endl;
    ++(*cnt);
    t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
    t->async_wait(std::bind(print, std::placeholders::_1, t, cnt));
  }
}

int main() {
  boost::asio::io_context io;
  int cnt = 0;
  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
  t.async_wait(std::bind(print, std::placeholders::_1, &t, &cnt));
  io.run();
}