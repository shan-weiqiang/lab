#include <boost/asio.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>

class printer {
public:
  printer(boost::asio::io_context &io, int cnt, int interval)
      : cnt{cnt}, interval{interval}, t{boost::asio::steady_timer(
                                          io, boost::asio::chrono::seconds(
                                                  interval))} {
    t.async_wait([this](boost::system::error_code ec) { print(ec); });
  }

  void print(boost::system::error_code) {
    if (cnt < 5) {
      std::cout << cnt << std::endl;
      ++cnt;
      //   reset timer start timepoint
      t.expires_at(t.expiry() + boost::asio::chrono::seconds(interval));
      t.async_wait([this](boost::system::error_code ec) { print(ec); });
    }
  }

  ~printer() { std::cout << "final cnt: " << cnt << std::endl; }

private:
  int cnt;
  int interval;
  boost::asio::steady_timer t;
};

int main() {
  boost::asio::io_context io;
  printer p{io, 0, 2};
  io.run();
}